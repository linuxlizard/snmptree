#include "stdafx.h"

#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>

char *passphrase = "(**TBURFO*(&no12";
int passphrase_len;

char *msg = "This is a test.";

#define BUFSIZE 1024

#define ENCRYPT_CIPHER EVP_des_ede3_cbc()

/* Lots of stuff taken from Chapter 4, pp.96-97 _Network Security with
 * OpenSSL_, Viega, Messier, and Chandra. O'Reilly 2001. ISBN 0-596-00270-X
 */

void hex_dump( unsigned char *ptr, int size )
{
    static char hex_ascii[] =
       { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
    int i;
    unsigned char line[80];
    unsigned char *ascii, *hex;
    unsigned char *endptr;
    unsigned long offset=0;

   endptr = ptr + size;
   memset( line, ' ', 80 );
   line[69] = 0;
   while( ptr != endptr ) {
      hex = &line[2];
      ascii = &line[52];
      for( i=0 ; i<16 ; i++ ) {
         if( isprint(*ptr) )
            *ascii++ = *ptr;
         else
            *ascii++ = '.';
         *hex++ = hex_ascii[ *ptr>>4 ];
         *hex++ = hex_ascii[ *ptr&0x0f ];
         *hex++ = ' ';
         ptr++;
         if( ptr == endptr ) {
            /* clean out whatever is left from the last line */
            memset( hex, ' ', (15-i)*3 );
            memset( ascii, ' ', 15-i );
            break;
         }
      }
      printf( "%#08lx %s\n", offset, line );
      offset += 16;
   }
}

int write_to_BIO( BIO *bio, char *out, int len ) 
{
    int total, written;
    int errcode;

    /* This loop writes the data to the file. It checks for errors as if the
     underlying file were non-blocking */
    for (total = 0; total < len; total += written) {
        if ((written = BIO_write (bio, out + total, len - total)) <= 0) {
            if (BIO_should_retry (bio)) {
                written = 0;
            }
            else {
                /* return the error */
                errcode = ERR_get_error();
                /* FIXME - do something with errcode? */
                return written;
            }
        }
    }

    /* Ensure all of our data is pushed all the way out to the destination */
    BIO_flush( bio );

    return total;
}

int read_from_BIO( BIO *bio, char *out, int len ) 
{
    int total, count;
    int errcode;

    /* This loop reads the data from the file. It checks for errors as if the
     underlying file were non-blocking */
    for (total = 0; total < len; total += count) {
        if ((count= BIO_read(bio, out + total, len - total)) <= 0) {
            errcode = ERR_get_error();
            /* FIXME - do something with errcode? */
//            if (BIO_should_retry (cipher)) {
//                written = 0;
//                continue;
//            }
            break;
        }
    }

    /* Ensure all of our data is pulled all the way from the source */
    BIO_flush (bio);

    return total;
}

int alloc_cipher_BIOs( BIO **buffer, BIO **b64, BIO **cipher )
{
    *buffer = *b64 = *cipher = NULL; 

    /* Create a buffering filter BIO to buffer writes to the file */
    *buffer = BIO_new (BIO_f_buffer ());
    if( buffer == NULL ) {
        return -1;
    }

    /* Create a base64 encoding filter BIO */
    *b64 = BIO_new (BIO_f_base64 ());
    if( *b64 == NULL ) {
        BIO_free( *buffer );
        *buffer = NULL;
        return -1;
    }
    
    *cipher = BIO_new (BIO_f_cipher ());
    if( *cipher == NULL ) {
        BIO_free( *buffer );
        BIO_free( *b64 );
        *buffer = *b64 = NULL;
        return -1;
    }

    /* success */
    return 0;
}

int write_cipher_data (const char *filename, char *out, int len, unsigned char *key, unsigned char *iv)
{
    int count, retcode;
    BIO *cipher, *b64, *buffer, *file;

    /* write_cipher_data() taken from Chapter 4, pp.96-97 _Network Security with
     * OpenSSL_, Viega, Messier, and Chandra. O'Reilly 2001. ISBN 0-596-00270-X
     *
     * (Changed the formatting a bit.)
     */

    /* Create a buffered file BIO for writing */
    file = BIO_new_file (filename, "w");
    if (!file) {
        /* FIXME - log an error */
        return -1;
    }

    retcode = alloc_cipher_BIOs( &buffer, &b64, &cipher );
    assert( retcode == 0 );
    if( retcode != 0 ) {
        return retcode;
    }

    BIO_set_cipher (cipher, ENCRYPT_CIPHER, key, iv, 1);

    /* Assemble the BIO chain to be in the order cipher-b64-buffer-file */
    BIO_push (cipher, b64);
    BIO_push (b64, buffer);
    BIO_push (buffer, file);

    count = write_to_BIO( cipher, out, len );
    assert( count == len );

    BIO_free_all (cipher);
    
    /* success */
    return count;
}

int read_cipher_data( const char *filename, char *out, int len, unsigned char *key, unsigned char *iv )
{
    int retcode, count;
    BIO *cipher, *b64, *buffer, *file;

    /* Create a buffered file BIO for writing */
    file = BIO_new_file (filename, "r");
    if (!file) {
        /* FIXME - log an error */
        return -1;
    }

    retcode = alloc_cipher_BIOs( &buffer, &b64, &cipher );
    assert( retcode == 0 );
    if( retcode != 0 ) {
        /* alloc_cipher_BIOs() should log error */
        return retcode;
    }

    BIO_set_cipher (cipher, ENCRYPT_CIPHER, key, iv, 0);

    /* Assemble the BIO chain to be in the order file-buffer-b64-cipher */
    BIO_push (cipher, b64);
    BIO_push (b64, buffer);
    BIO_push (buffer, file);

    count = read_from_BIO( cipher, out, len );
    assert( count > 0 );

    BIO_free_all( cipher );
    
    /* success */
    return count;
}

int write_b64( const char *filename, unsigned char *out, int len )
{
    int count;
    BIO *b64, *file;

    /* Create a buffered file BIO for writing */
    file = BIO_new_file (filename, "w");
    if (!file) {
        /* FIXME - log an error */
        return -1;
    }

    /* Create a base64 encoding filter BIO */
    b64 = BIO_new (BIO_f_base64 ());
    assert( b64 != NULL );
    if( b64 == NULL ) {
        /* FIXME - log an error */
        return -1;
    }

    /* Assemble the BIO chain to be in the order b64-file */
    BIO_push (b64, file );

    count = write_to_BIO( b64, out, len );
    assert( count == len );

    BIO_free_all( b64 );

    /* success */
    return count;
}

int read_b64( const char *filename, unsigned char *out, int len )
{
    int count;
    BIO *b64, *buffer, *file;

    /* Create a buffered file BIO for reading */
    file = BIO_new_file (filename, "r");
    if (!file) {
        /* FIXME - log an error */
        return -1;
    }

    /* Create a base64 encoding filter BIO */
    b64 = BIO_new (BIO_f_base64 ());
    if( b64 == NULL ) {
        /* FIXME - log an error */
        return -1;
    }

    buffer = BIO_new (BIO_f_buffer ());

    /* Assemble the BIO chain to be in the order b64-file */
    BIO_push (b64, buffer );
    BIO_push( buffer, file );

    /* load the data */
    count = read_from_BIO( b64, out, len );
    assert( count == len );

    BIO_free_all( b64 );

    /* success */
    return count;
}

#ifdef TEST_ME
#include <stdio.h>
int main( void )
{
    int retcode;
    char outbuf[BUFSIZE];
    unsigned char iv[] = {1,2,3,4,5,6,7,8};
    unsigned char new_iv[8];

    RAND_pseudo_bytes( iv, 8 );
    printf( "iv1:\n" );
    hex_dump( iv, 8 );

    retcode = write_cipher_data( "out.dat", msg, strlen(msg), passphrase, iv );
    assert( retcode == strlen(msg) );
    printf( "iv2:\n" );
    hex_dump( iv, 8 );

    retcode = write_b64( "out.dat.iv", iv, 8 );
    assert( retcode == 8 );
    memset( new_iv, 0, sizeof(new_iv) );
    retcode = read_b64( "out.dat.iv", new_iv, 8 );
    assert( retcode == 8 );
    printf( "new_iv:\n" );
    hex_dump( new_iv, 8 );
    assert( memcmp( iv, new_iv, 8 ) == 0 );
    
    memset( outbuf, 0, BUFSIZE );
    retcode = read_cipher_data( "out.dat", outbuf, BUFSIZE, passphrase, iv );
    assert( retcode == strlen(msg) );
    printf( "iv3:\n" );
    hex_dump( iv, 8 );
    printf( "outbuf:\n" );
    hex_dump( outbuf, 64 );
    assert( strncmp( msg, outbuf, strlen(msg) ) == 0 );

    printf( "success!\n" );
    return 0;
}
#endif

