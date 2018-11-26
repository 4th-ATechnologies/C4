//
//  testKeys.c
//  S4
//
//  Created by vincent Moscaritolo on 11/9/15.
//  Copyright © 2015 4th-A Technologies, LLC. All rights reserved.
//

#include <stdio.h>
#include <time.h>
#include "optest.h"


static char *const kS4KeyProp_TestPassCodeID   = "passcodeID";
static char *const kS4KeyProp_Owner    = "owner-id";

typedef struct  {
    char*               comment;
    Cipher_Algorithm    algor;
    int                 keysize;
    uint8_t             *key;
    uint8_t             *passPhrase;
    
} cipherKATvector;


typedef struct  {
	char*               comment;
	Cipher_Algorithm    algor;
	int                 keysize;
	uint8_t             *key;
	uint8_t             *passPhrase;
	P2K_Algorithm		p2KAlgor;
} cipherP2K_KATvector;


static char *exported_keys   = NULL;
static int exported_key_count   = 0;

static S4Err sCompareKeys( S4KeyContext  *keyCtx, S4KeyContext  *keyCtx1, bool ignorePrivPub)
{
    S4Err err = kS4Err_NoErr;
    
    ValidateParam(keyCtx);
    ValidateParam(keyCtx1);
    
    S4KeyType           type1,type2;
    Cipher_Algorithm     algor1, algor2;

    HASH_Algorithm     hash1, hash2;

    int8_t      key1[128], key2[128];
    size_t      keyLen1, keyLen2;
    
    time_t          date1,date2;
    
    uint8_t         keyHash1[kS4KeyPBKDF2_HashBytes] = {0};
    uint8_t         keyHash2[kS4KeyPBKDF2_HashBytes] = {0};

    uint8_t         keyID1[kS4Key_KeyIDBytes] = {0};
    uint8_t         keyID2[kS4Key_KeyIDBytes] = {0};

    err = S4Key_GetProperty(keyCtx, kS4KeyProp_KeyType, NULL, &type1, sizeof(type1), NULL ); CKERR;
    err = S4Key_GetProperty(keyCtx1, kS4KeyProp_KeyType, NULL, &type2, sizeof(type1), NULL ); CKERR;
    ASSERTERR(type1 == type2,  kS4Err_SelfTestFailed);

    switch (type1) {
        case kS4KeyType_Symmetric:
            
            err = S4Key_GetProperty(keyCtx, kS4KeyProp_KeySuite, NULL, &algor1, sizeof(algor1), NULL ); CKERR;
            err = S4Key_GetProperty(keyCtx1, kS4KeyProp_KeySuite, NULL, &algor2, sizeof(algor2), NULL ); CKERR;
            ASSERTERR(algor1 == algor2,  kS4Err_SelfTestFailed);
            
            err = S4Key_GetProperty(keyCtx, kS4KeyProp_KeyData, NULL, &key1 , sizeof(key1), &keyLen1 ); CKERR;
            err = S4Key_GetProperty(keyCtx1, kS4KeyProp_KeyData, NULL, &key2 , sizeof(key2), &keyLen2 ); CKERR;
            ASSERTERR(keyLen1 == keyLen2,  kS4Err_SelfTestFailed);
            err = compareResults( key1, key2, keyLen1,
                                 kResultFormat_Byte, "Symmetric key"); CKERR;
            
            err = S4Key_GetProperty(keyCtx, kS4KeyProp_Mac, NULL, &keyHash1, sizeof(keyHash1), NULL ); CKERR;
            err = S4Key_GetProperty(keyCtx1, kS4KeyProp_Mac, NULL, &keyHash2, sizeof(keyHash2), NULL ); CKERR;
            
            err = compareResults( keyHash1, keyHash1, kS4KeyPBKDF2_HashBytes,
                                 kResultFormat_Byte, "KeyHash"); CKERR;
            
            err = S4Key_GetProperty(keyCtx, kS4KeyProp_KeyID, NULL, &keyID1, sizeof(keyID1), NULL ); CKERR;
            err = S4Key_GetProperty(keyCtx1, kS4KeyProp_KeyID, NULL, &keyID2, sizeof(keyID2), NULL ); CKERR;
            
            err = compareResults( keyID1, keyID2, kS4Key_KeyIDBytes,
                                 kResultFormat_Byte, "keyID"); CKERR;

             break;
            
        case kS4KeyType_Tweekable:
            
            err = S4Key_GetProperty(keyCtx, kS4KeyProp_KeySuite, NULL, &algor1, sizeof(algor1), NULL ); CKERR;
            err = S4Key_GetProperty(keyCtx1, kS4KeyProp_KeySuite, NULL, &algor2, sizeof(algor2), NULL ); CKERR;
            ASSERTERR(algor1 == algor2,  kS4Err_SelfTestFailed);

            err = S4Key_GetProperty(keyCtx, kS4KeyProp_KeyData, NULL, &key1 , sizeof(key1), &keyLen1 ); CKERR;
            err = S4Key_GetProperty(keyCtx1, kS4KeyProp_KeyData, NULL, &key2 , sizeof(key2), &keyLen2 ); CKERR;
            ASSERTERR(keyLen1 == keyLen2,  kS4Err_SelfTestFailed);
            err = compareResults( key1, key2, keyLen1,
                                 kResultFormat_Byte, "TBC key"); CKERR;
            
            err = S4Key_GetProperty(keyCtx, kS4KeyProp_Mac, NULL, &keyHash1, sizeof(keyHash1), NULL ); CKERR;
            err = S4Key_GetProperty(keyCtx1, kS4KeyProp_Mac, NULL, &keyHash2, sizeof(keyHash2), NULL ); CKERR;
            
            err = compareResults( keyHash1, keyHash1, kS4KeyPBKDF2_HashBytes,
                                 kResultFormat_Byte, "KeyHash"); CKERR;
            
            err = S4Key_GetProperty(keyCtx, kS4KeyProp_KeyID, NULL, &keyID1, sizeof(keyID1), NULL ); CKERR;
            err = S4Key_GetProperty(keyCtx1, kS4KeyProp_KeyID, NULL, &keyID2, sizeof(keyID2), NULL ); CKERR;
            
            err = compareResults( keyID1, keyID2, kS4Key_KeyIDBytes,
                                 kResultFormat_Byte, "keyID"); CKERR;

            break;
    
        case kS4KeyType_Share:
            ASSERTERR(keyCtx->share.threshold == keyCtx1->share.threshold,  kS4Err_SelfTestFailed);
            ASSERTERR(keyCtx->share.xCoordinate == keyCtx1->share.xCoordinate,  kS4Err_SelfTestFailed);
            ASSERTERR(keyCtx->share.shareSecretLen == keyCtx1->share.shareSecretLen,  kS4Err_SelfTestFailed);
      
            err = compareResults( keyCtx->share.shareHash, keyCtx1->share.shareHash, kS4ShareInfo_HashBytes,
                                 kResultFormat_Byte, "Share Hash"); CKERR;
            
            
            err = compareResults( keyCtx->share.shareSecret, keyCtx1->share.shareSecret, keyCtx->share.shareSecretLen,
                                 kResultFormat_Byte, "Share Hash"); CKERR;
  
            err = S4Key_GetProperty(keyCtx, kS4KeyProp_Mac, NULL, &keyHash1, sizeof(keyHash1), NULL ); CKERR;
            err = S4Key_GetProperty(keyCtx1, kS4KeyProp_Mac, NULL, &keyHash2, sizeof(keyHash2), NULL ); CKERR;
            
            err = compareResults( keyHash1, keyHash1, kS4KeyPBKDF2_HashBytes,
                                 kResultFormat_Byte, "KeyHash"); CKERR;
  
            break;
            

        case kS4KeyType_PublicKey:
            err = S4Key_GetProperty(keyCtx, kS4KeyProp_KeySuite, NULL, &algor1, sizeof(algor1), NULL ); CKERR;
            err = S4Key_GetProperty(keyCtx1, kS4KeyProp_KeySuite, NULL, &algor2, sizeof(algor2), NULL ); CKERR;
            ASSERTERR(algor1 == algor2,  kS4Err_SelfTestFailed);
    
            if(!ignorePrivPub)
            {
                ASSERTERR(keyCtx->pub.isPrivate == keyCtx1->pub.isPrivate,  kS4Err_SelfTestFailed);
            }
            
            err = S4Key_GetProperty(keyCtx, kS4KeyProp_KeyID, NULL, &keyID1, sizeof(keyID1), NULL ); CKERR;
            err = S4Key_GetProperty(keyCtx1, kS4KeyProp_KeyID, NULL, &keyID2, sizeof(keyID2), NULL ); CKERR;
            
            err = compareResults( keyID1, keyID2, kS4Key_KeyIDBytes,
                                 kResultFormat_Byte, "keyID"); CKERR;

            if(keyCtx->pub.isPrivate && keyCtx1->pub.isPrivate)
            {
              // we could compare the MAC on private keys.
            }
            
            break;
            
        case kS4KeyType_PBKDF2:
            switch (keyCtx->pbkdf2.keyAlgorithmType)
        {
            case kS4KeyType_Symmetric:
                 break;
                
            case kS4KeyType_Tweekable:
                 break;
                
            default:;
                
        };
            break;
         
        case kS4KeyType_Signature:
        {
            err = S4Key_GetProperty(keyCtx, kS4KeyProp_HashAlgorithm, NULL, &hash1, sizeof(hash1), NULL ); CKERR;
            err = S4Key_GetProperty(keyCtx1, kS4KeyProp_HashAlgorithm, NULL, &hash2, sizeof(hash2), NULL ); CKERR;
            ASSERTERR(hash1 == hash2,  kS4Err_SelfTestFailed);
           
            err = S4Key_GetProperty(keyCtx, kS4KeyProp_SignedBy, NULL, &keyID1, sizeof(keyID1), NULL ); CKERR;
            err = S4Key_GetProperty(keyCtx1, kS4KeyProp_SignedBy, NULL, &keyID2, sizeof(keyID2), NULL ); CKERR;
            
            err = compareResults( keyID1, keyID2, kS4Key_KeyIDBytes,
                                 kResultFormat_Byte, "SignedBy"); CKERR;
            
            err = S4Key_GetProperty(keyCtx, kS4KeyProp_SignedDate, NULL,  &date1, sizeof(time_t),NULL); CKERR;
            err = S4Key_GetProperty(keyCtx1, kS4KeyProp_SignedDate, NULL, &date2, sizeof(time_t), NULL); CKERR;
            ASSERTERR(date1 == date2,  kS4Err_SelfTestFailed);
  
            err = S4Key_GetProperty(keyCtx, kS4KeyProp_SigExpire, NULL,  &date1, sizeof(time_t),NULL); CKERR;
            err = S4Key_GetProperty(keyCtx1, kS4KeyProp_SigExpire, NULL, &date2, sizeof(time_t), NULL); CKERR;
            ASSERTERR(date1 == date2,  kS4Err_SelfTestFailed);
            
            err = S4Key_GetProperty(keyCtx, kS4KeyProp_SigID, NULL, &keyID1, sizeof(keyID1), NULL ); CKERR;
            err = S4Key_GetProperty(keyCtx1,kS4KeyProp_SigID, NULL, &keyID2, sizeof(keyID2), NULL ); CKERR;
            
            err = compareResults( keyID1, keyID2, kS4Key_KeyIDBytes,
                                 kResultFormat_Byte, "SigID"); CKERR;
            
        }
            
            break;
            
        default:
            err = kS4Err_UnknownError;
            break;
    }

    // compare additional properties
    S4KeyProperty* prop = keyCtx->propList;
    while(prop)
    {
        S4KeyPropertyType type2 = S4KeyPropertyType_Invalid;
        void     *data2 = NULL;
        size_t      data2Len = 0;
 
        S4KeyPropertyType type1 = S4KeyPropertyType_Invalid;
        void     *data1 = NULL;
        size_t      data1Len = 0;
        
        err = S4Key_GetAllocatedProperty(keyCtx, (const char*) prop->prop, &type1, &data1, &data1Len); CKERR;
        err = S4Key_GetAllocatedProperty(keyCtx1, (const char*) prop->prop, &type2, &data2, &data2Len); CKERR;
        
        ASSERTERR(type1 == type2,  kS4Err_SelfTestFailed);
        
        err = compare2Results( data1, data1Len, data2, data2Len, kResultFormat_Byte, ( char*) prop->prop); CKERR;
        
        if(data1) free(data1);
        if(data2) free(data2);
        
        prop = prop->next;
    }

done:
    return err;
}



static S4Err sRunCipherP2KImportExportKAT(  cipherP2K_KATvector *kat)
{
	S4Err err = kS4Err_NoErr;
	S4KeyContextRef keyCtx =  kInvalidS4KeyContextRef;
	S4KeyContextRef keyCtx1 =  kInvalidS4KeyContextRef;

	S4KeyContextRef  *passCtx = NULL;
	size_t      keyCount = 0;

	const char* name = NULL;

	uint8_t     *data = NULL;
	size_t      dataLen = 0;

	time_t          testDate  = time(NULL) ;

	name = cipher_algor_table(kat->algor);

	OPTESTLogVerbose("\t%-14s ", name);
	OPTESTLogVerbose("%8s", "Export");

	err = S4Key_NewSymmetric(kat->algor, kat->key, &keyCtx  ); CKERR;

	err = S4Key_SetProperty(keyCtx,kS4KeyProp_TestPassCodeID,S4KeyPropertyType_UTF8String, kat->comment, strlen(kat->comment)); CKERR;
	err = S4Key_SetProperty(keyCtx, kS4KeyProp_StartDate, S4KeyPropertyType_Time ,  &testDate, sizeof(time_t)); CKERR;

	err = S4Key_SerializeToPassPhrase_WithAlgorithm(kat->p2KAlgor,
													keyCtx,
													kat->passPhrase, strlen((char*)kat->passPhrase),
													&data, &dataLen); CKERR;

	OPTESTLogDebug("\n------\n%s------\n",data);

	OPTESTLogVerbose("%8s", "Import");
	err = S4Key_DeserializeKeys(data, dataLen, &keyCount, &passCtx ); CKERR;

	//   sDumpS4Key(OPTESTLOG_LEVEL_DEBUG, passCtx);
//
	OPTESTLogVerbose("%8s", "Verify");
	err = S4Key_VerifyPassPhrase(passCtx[0], kat->passPhrase, strlen((char*)kat->passPhrase)); CKERR;

	err = S4Key_DecryptFromPassPhrase(passCtx[0],kat->passPhrase, strlen((char*)kat->passPhrase), &keyCtx1); CKERR;

	err = sCompareKeys(keyCtx, keyCtx1, false); CKERR;

	if(data)
	{
		asprintf(&exported_keys,"%s %s %s", exported_keys,strlen(exported_keys) > 1?",":"", data );
		exported_key_count++;
	}

done:
	if(data)
		XFREE(data);

	if(S4KeyContextRefIsValid(keyCtx))
	{
		S4Key_Free(keyCtx);
	}
	if(passCtx)
	{
		if(S4KeyContextRefIsValid(passCtx[0]))
		{
			S4Key_Free(passCtx[0]);
		}
		XFREE(passCtx);

	}
	if(S4KeyContextRefIsValid(keyCtx1))
	{
		S4Key_Free(keyCtx1);
	}

	OPTESTLogVerbose("\n");

	return err;
}

static S4Err sRunCipherPBKDF2ImportExportKAT(  cipherKATvector *kat)
{
    S4Err err = kS4Err_NoErr;
    S4KeyContextRef keyCtx =  kInvalidS4KeyContextRef;
    S4KeyContextRef keyCtx1 =  kInvalidS4KeyContextRef;
    
    S4KeyContextRef  *passCtx = NULL;
    size_t      keyCount = 0;
     
    const char* name = NULL;
    
    uint8_t     *data = NULL;
    size_t      dataLen = 0;
    
    time_t          testDate  = time(NULL) ;
    
    name = cipher_algor_table(kat->algor);
    
    OPTESTLogVerbose("\t%-14s ", name);
    OPTESTLogVerbose("%8s", "Export");
    
    err = S4Key_NewSymmetric(kat->algor, kat->key, &keyCtx  ); CKERR;
    
    err = S4Key_SetProperty(keyCtx,kS4KeyProp_TestPassCodeID,S4KeyPropertyType_UTF8String, kat->comment, strlen(kat->comment)); CKERR;
    err = S4Key_SetProperty(keyCtx, kS4KeyProp_StartDate, S4KeyPropertyType_Time ,  &testDate, sizeof(time_t)); CKERR;
  
    err = S4Key_SerializeToPassPhrase(keyCtx, kat->passPhrase, strlen((char*)kat->passPhrase), &data, &dataLen); CKERR;
    
      OPTESTLogDebug("\n------\n%s------\n",data);
    
    OPTESTLogVerbose("%8s", "Import");
    err = S4Key_DeserializeKeys(data, dataLen, &keyCount, &passCtx ); CKERR;
    
 //   sDumpS4Key(OPTESTLOG_LEVEL_DEBUG, passCtx);
    
    OPTESTLogVerbose("%8s", "Verify");
    err = S4Key_VerifyPassPhrase(passCtx[0], kat->passPhrase, strlen((char*)kat->passPhrase)); CKERR;

    err = S4Key_DecryptFromPassPhrase(passCtx[0],kat->passPhrase, strlen((char*)kat->passPhrase), &keyCtx1); CKERR;
    
    err = sCompareKeys(keyCtx, keyCtx1, false); CKERR;

    if(data)
    {
        asprintf(&exported_keys,"%s %s %s", exported_keys,strlen(exported_keys) > 1?",":"", data );
        exported_key_count++;
     }
    
done:
    if(data)
        XFREE(data);
 
    if(S4KeyContextRefIsValid(keyCtx))
    {
        S4Key_Free(keyCtx);
    }
    if(passCtx)
    {
        if(S4KeyContextRefIsValid(passCtx[0]))
        {
             S4Key_Free(passCtx[0]);
        }
        XFREE(passCtx);
        
    }
      if(S4KeyContextRefIsValid(keyCtx1))
    {
        S4Key_Free(keyCtx1);
    }
  
    OPTESTLogVerbose("\n");

    return err;
}

static S4Err sRunCipherImportExportKAT(  cipherKATvector *kat)
{
    S4Err err = kS4Err_NoErr;
    S4KeyContextRef passKeyCtx =  kInvalidS4KeyContextRef;
    S4KeyContextRef keyCtx =  kInvalidS4KeyContextRef;
    S4KeyContextRef keyCtx1 =  kInvalidS4KeyContextRef;
    
    S4KeyContextRef  *importCtx = NULL;
    size_t      keyCount = 0;
    
    const char* name = NULL;
    
    uint8_t     *data = NULL;
    size_t      dataLen = 0;
    
    time_t          testDate  = time(NULL) ;
    
    name = cipher_algor_table(kat->algor);
    
    OPTESTLogVerbose("\t%-14s ", name);
    OPTESTLogVerbose("%8s", "Export");
    
    err = S4Key_NewKey(kCipher_Algorithm_2FISH256, &passKeyCtx); CKERR;
    
    err = S4Key_NewSymmetric(kat->algor, kat->key, &keyCtx  ); CKERR;
    
    err = S4Key_SetProperty(keyCtx,kS4KeyProp_TestPassCodeID,S4KeyPropertyType_UTF8String, kat->comment, strlen(kat->comment)); CKERR;
    err = S4Key_SetProperty(keyCtx, kS4KeyProp_StartDate, S4KeyPropertyType_Time ,  &testDate, sizeof(time_t)); CKERR;
    
    err = S4Key_SerializeToS4Key(keyCtx, passKeyCtx, &data, &dataLen); CKERR;
    
    OPTESTLogDebug("\n------\n%s------\n",data);
    
    OPTESTLogVerbose("%8s", "Import");
    err = S4Key_DeserializeKeys(data, dataLen, &keyCount, &importCtx ); CKERR;
    ASSERTERR(keyCount == 1,  kS4Err_SelfTestFailed);
    
    OPTESTLogVerbose("%8s", "Verify");
    err = S4Key_DecryptFromS4Key(importCtx[0], passKeyCtx , &keyCtx1); CKERR;
    
    err = sCompareKeys(keyCtx, keyCtx1, false); CKERR;
    
    
    if(data)
    {
        asprintf(&exported_keys,"%s %s %s", exported_keys,strlen(exported_keys) > 1?",":"", data );
        exported_key_count++;
    }
    
done:
    if(data)
        XFREE(data);
    
    if(S4KeyContextRefIsValid(keyCtx))
    {
        S4Key_Free(keyCtx);
    }

    if(importCtx)
    {
        if(S4KeyContextRefIsValid(importCtx[0]))
        {
            S4Key_Free(importCtx[0]);
        }
        XFREE(importCtx);
        
    }
    
    if(S4KeyContextRefIsValid(passKeyCtx))
    {
        S4Key_Free(passKeyCtx);
    }
 
    if(S4KeyContextRefIsValid(keyCtx1))
    {
        S4Key_Free(keyCtx1);
    }
    
    OPTESTLogVerbose("\n");
    
    return err;
}


static S4Err  sTestSymmetricKeys()
{
    S4Err     err = kS4Err_NoErr;
    int i;
    
    uint8_t* passPhrase1 = (uint8_t*)"Tant las fotei com auziretz";
   
    /* AES 128 bit key */
    uint8_t K1[] = {
        0x00, 0x01, 0x02, 0x03, 0x05, 0x06, 0x07, 0x08,
        0x0A, 0x0B, 0x0C, 0x0D, 0x0F, 0x10, 0x11, 0x12
    };
    
    /* AES 192 bit key */
    uint8_t K2[] = {
        0x00, 0x01, 0x02, 0x03, 0x05, 0x06, 0x07, 0x08,
        0x0A, 0x0B, 0x0C, 0x0D, 0x0F, 0x10, 0x11, 0x12,
        0x14, 0x15, 0x16, 0x17, 0x19, 0x1A, 0x1B, 0x1C
    };
    
    /* AES 256 bit key */
    uint8_t K3[] = {
        0x00, 0x01, 0x02, 0x03, 0x05, 0x06, 0x07, 0x08,
        0x0A, 0x0B, 0x0C, 0x0D, 0x0F, 0x10, 0x11, 0x12,
        0x14, 0x15, 0x16, 0x17, 0x19, 0x1A, 0x1B, 0x1C,
        0x1E, 0x1F, 0x20, 0x21, 0x23, 0x24, 0x25, 0x26
    };
    
    cipherKATvector kat_vector_array[] =
    {
        {"Key 1",	kCipher_Algorithm_AES128, 128,	K1, passPhrase1},
        {"Key 2",    kCipher_Algorithm_AES192, 192,	K2, passPhrase1},
        {"Key 3",   kCipher_Algorithm_AES256, 256,   K3, passPhrase1},
        {"Key 4",   kCipher_Algorithm_2FISH256, 256,   K3, passPhrase1},
    };

	cipherP2K_KATvector kat_vector_array1[] =
	{
		{"Key 1",	kCipher_Algorithm_AES128, 128,	K1, passPhrase1, kP2K_Algorithm_PBKDF2},
		{"Key 1",	kCipher_Algorithm_AES128, 128,	K1, passPhrase1, kP2K_Algorithm_Argon2d},
		{"Key 1",	kCipher_Algorithm_AES128, 128,	K1, passPhrase1, kP2K_Algorithm_Argon2i},
		{"Key 1",	kCipher_Algorithm_AES128, 128,	K1, passPhrase1, kP2K_Algorithm_Argon2id},

		{"Key 2",	kCipher_Algorithm_AES192, 192,	K2, passPhrase1, kP2K_Algorithm_PBKDF2},
		{"Key 2",	kCipher_Algorithm_AES192, 192,	K2, passPhrase1, kP2K_Algorithm_Argon2d},
		{"Key 2",	kCipher_Algorithm_AES192, 192,	K2, passPhrase1, kP2K_Algorithm_Argon2i},
		{"Key 2",	kCipher_Algorithm_AES192, 192,	K2, passPhrase1, kP2K_Algorithm_Argon2id},

		{"Key 3",   kCipher_Algorithm_AES256, 256,   K3, passPhrase1, kP2K_Algorithm_PBKDF2},
		{"Key 3",   kCipher_Algorithm_AES256, 256,   K3, passPhrase1, kP2K_Algorithm_Argon2d},
		{"Key 3",   kCipher_Algorithm_AES256, 256,   K3, passPhrase1, kP2K_Algorithm_Argon2i},
		{"Key 3",   kCipher_Algorithm_AES256, 256,   K3, passPhrase1, kP2K_Algorithm_Argon2id},

 		{"Key 4",   kCipher_Algorithm_2FISH256, 256,   K3, passPhrase1, kP2K_Algorithm_PBKDF2},
  		{"Key 4",   kCipher_Algorithm_2FISH256, 256,   K3, passPhrase1, kP2K_Algorithm_Argon2d},
		{"Key 4",   kCipher_Algorithm_2FISH256, 256,   K3, passPhrase1, kP2K_Algorithm_Argon2i},
		{"Key 4",   kCipher_Algorithm_2FISH256, 256,   K3, passPhrase1, kP2K_Algorithm_Argon2id},
	};


	OPTESTLogInfo("\nTesting Symmetric S4Key Encoding\n");

    /* run  known answer tests (KAT) */
    for (i = 0; i < sizeof(kat_vector_array)/ sizeof(cipherKATvector) ; i++)
    {
        err = sRunCipherImportExportKAT( &kat_vector_array[i] ); CKERR;
    }


    OPTESTLogInfo("\nTesting Symmetric S4Key (PBKDF2) Encode to passphrase\n");

    /* run  known answer tests (KAT) */
    for (i = 0; i < sizeof(kat_vector_array)/ sizeof(cipherKATvector) ; i++)
    {
        err = sRunCipherPBKDF2ImportExportKAT( &kat_vector_array[i] ); CKERR;
	}

	OPTESTLogInfo("\nTesting Symmetric S4Key Encode to passphrase\n");

	/* run  known answer tests (KAT) */
	for (i = 0; i < sizeof(kat_vector_array1)/ sizeof(cipherP2K_KATvector) ; i++)
	{
		err = sRunCipherP2KImportExportKAT( &kat_vector_array1[i] ); CKERR;
	}


done:
     return err;
}




static S4Err sRunSharedPBKDF2ImportExportKAT(  cipherKATvector *kat)
{
    
#define kNumShares				8
#define kShareThreshold			6
    
    S4Err               err = kS4Err_NoErr;
    
    SHARES_ContextRef   ctx  			= kInvalidSHARES_ContextRef;
    SHARES_ShareInfo*   shares[kNumShares] = {NULL};
    SHARES_ShareInfo*   recoveredShares[kNumShares] = {NULL};
    
    S4KeyContextRef     shareCtx[kNumShares] = {kInvalidS4KeyContextRef};
    uint8_t*            shareData[kNumShares] = {NULL};
    
    S4KeyContextRef *encodedCtx =  NULL;
    size_t          keyCount = 0;
    
    uint8_t             PT1[128];
    size_t              PT1len;
    
    int                 i;
    const char* name = NULL;
   
    
	name = cipher_algor_table(kat->algor);
    
    OPTESTLogVerbose("\t%-14s ", name);
    // skip AES-192
    if(kat->algor == kCipher_Algorithm_AES192)
    {
        OPTESTLogVerbose("%s", " -- Not Supported --");
        goto done;
 
    }
    
    OPTESTLogVerbose("%s", "Split");
    
    err = SHARES_Init( kat->key, kat->keysize >>3 ,
                      kNumShares,
                      kShareThreshold,
                      &ctx); CKERR;
    
    OPTESTLogVerbose("%8s", "Export");
    for(i = 0; i < kNumShares; i++)
    {
        size_t shareLen = 0;
        
        err = SHARES_GetShareInfo(ctx, i, &shares[i], &shareLen); CKERR;
        err = S4Key_NewShare( shares[i], &shareCtx[i]); CKERR;
        
        err = S4Key_SetProperty(shareCtx[i],kS4KeyProp_TestPassCodeID,S4KeyPropertyType_UTF8String, kat->comment, strlen(kat->comment)); CKERR;
        err = S4Key_SerializeToPassPhrase(shareCtx[i], kat->passPhrase, strlen((char*)kat->passPhrase),&shareData[i], NULL); CKERR;
        
        
        OPTESTLogDebug("\n------\n%s",shareData[i]);
        
        if(shareData[i])
        {
            asprintf(&exported_keys,"%s %s %s", exported_keys,strlen(exported_keys) > 1?",":"", shareData[i] );
            exported_key_count++;
        }

    }
    
    OPTESTLogVerbose("%8s", "Decode");
    for(i = 0; i < kNumShares; i++)
    {
        S4KeyContextRef decodedCtx =  kInvalidS4KeyContextRef;
        
        err = S4Key_DeserializeKeys(shareData[i], strlen((char*)shareData[i]), &keyCount, &encodedCtx ); CKERR;
        ASSERTERR(keyCount == 1,  kS4Err_SelfTestFailed);
        
        err = S4Key_DecryptFromPassPhrase(encodedCtx[0],kat->passPhrase, strlen((char*)kat->passPhrase), &decodedCtx); CKERR;
        
        err = sCompareKeys(decodedCtx, shareCtx[i], false); CKERR;
        
        recoveredShares[i] = XMALLOC(sizeof(SHARES_ShareInfo)); CKNULL(recoveredShares[i]);
        COPY(&decodedCtx->share, recoveredShares[i], sizeof(SHARES_ShareInfo));
        
        S4Key_Free(decodedCtx);
        
        S4Key_Free(encodedCtx[0]);
        XFREE(encodedCtx);
        encodedCtx = NULL;
    }
    
    OPTESTLogVerbose("%14s", "Reconstruct");
    err = SHARES_CombineShareInfo(kNumShares, recoveredShares, PT1, sizeof(PT1), &PT1len); CKERR;
    
    err = compare2Results( kat->key, kat->keysize >>3 ,PT1 ,PT1len, kResultFormat_Byte, "reconstructed key"); CKERR;
    
done:

    if(encodedCtx)
    {
        if(S4KeyContextRefIsValid(encodedCtx[0]))
        {
            S4Key_Free(encodedCtx[0]);
        }
        XFREE(encodedCtx);
    }
    
    
    for(i = 0; i < kNumShares; i++)
    {
        if(shares[i])
            XFREE(shares[i]);
        
        if(S4KeyContextRefIsValid(shareCtx[i]))
            S4Key_Free(shareCtx[i]);
        
        if(shareData[i])
            XFREE(shareData[i]);
        
        if(recoveredShares[i])
            XFREE(recoveredShares[i]);
        
    }
    
    if(SHARES_ContextRefIsValid(ctx))
        SHARES_Free(ctx);
    
    OPTESTLogVerbose("\n");
    
    return err;
}

static S4Err sRunSharedECCImportExportKAT(  cipherKATvector *kat)
{
    
#define kNumShares				8
#define kShareThreshold			6
    
    S4Err               err = kS4Err_NoErr;
    ECC_ContextRef      eccPub = kInvalidECC_ContextRef;
    ECC_ContextRef      eccPriv = kInvalidECC_ContextRef;
    S4KeyContextRef     pubCtx =  kInvalidS4KeyContextRef;
    S4KeyContextRef     privCtx=  kInvalidS4KeyContextRef;
    
    
    SHARES_ContextRef   ctx  			= kInvalidSHARES_ContextRef;
    SHARES_ShareInfo*   shares[kNumShares] = {NULL};
    SHARES_ShareInfo*   recoveredShares[kNumShares] = {NULL};
    
    S4KeyContextRef     shareCtx[kNumShares] = {kInvalidS4KeyContextRef};
    uint8_t*            shareData[kNumShares] = {NULL};
    
    S4KeyContextRef *encodedCtx =  NULL;
    size_t          keyCount = 0;
    
    uint8_t             keyID[kS4Key_KeyIDBytes]  = {0};
    size_t              keyIDLen = 0;
    
    uint8_t             keyID1[kS4Key_KeyIDBytes] = {0};
    size_t              keyIDLen1 = 0;
    
    uint8_t             PT1[128];
    size_t              PT1len;
    
    int                 i;
	const char* name = NULL;

    
    uint8_t ecc414_pubkey[] = {
        0x04,0x06,0x8b,0x14,0xa4,0x14,0x6a,0x2a,
        0x3d,0xab,0x05,0xda,0xdf,0x75,0xef,0x5f,
        0xaf,0x7c,0xbf,0x8e,0x92,0x75,0x6c,0xe4,
        0x9f,0x93,0x69,0x6e,0x42,0x15,0x2e,0x9d,
        0xb2,0xde,0xd7,0xf0,0x79,0xbe,0xb6,0x12,
        0x1a,0x73,0x70,0x17,0x15,0x93,0x6f,0xa4,
        0x2c,0xbf,0x21,0x99,0xb6,0x23,0xa7,0xb7,
        0x0e,0x15,0x35,0x0d,0xf5,0x0e,0xc7,0xa0,
        0x2e,0xcf,0x66,0xac,0x65,0x3b,0x5c,0xf6,
        0x19,0xa1,0xdb,0x16,0x41,0x7f,0xef,0xb4,
        0x19,0x6a,0xd1,0xa4,0x91,0x4c,0x4e,0x6a,
        0x11,0xb6,0xfd,0xfa,0x90,0x11,0x13,0x10,
        0x0f,0x64,0xaf,0x65,0x0a,0x74,0x85,0x53,
        0x0d};
    
    
    uint8_t ecc414_privkey[] = {
        0x30,0x81,0xa9,0x03,0x02,0x07,0x80,0x02,
        0x01,0x34,0x02,0x34,0x06,0x8b,0x14,0xa4,
        0x14,0x6a,0x2a,0x3d,0xab,0x05,0xda,0xdf,
        0x75,0xef,0x5f,0xaf,0x7c,0xbf,0x8e,0x92,
        0x75,0x6c,0xe4,0x9f,0x93,0x69,0x6e,0x42,
        0x15,0x2e,0x9d,0xb2,0xde,0xd7,0xf0,0x79,
        0xbe,0xb6,0x12,0x1a,0x73,0x70,0x17,0x15,
        0x93,0x6f,0xa4,0x2c,0xbf,0x21,0x99,0xb6,
        0x02,0x34,0x23,0xa7,0xb7,0x0e,0x15,0x35,
        0x0d,0xf5,0x0e,0xc7,0xa0,0x2e,0xcf,0x66,
        0xac,0x65,0x3b,0x5c,0xf6,0x19,0xa1,0xdb,
        0x16,0x41,0x7f,0xef,0xb4,0x19,0x6a,0xd1,
        0xa4,0x91,0x4c,0x4e,0x6a,0x11,0xb6,0xfd,
        0xfa,0x90,0x11,0x13,0x10,0x0f,0x64,0xaf,
        0x65,0x0a,0x74,0x85,0x53,0x0d,0x02,0x34,
        0x2b,0x30,0xd2,0xe0,0x76,0xfd,0x09,0x6b,
        0xcc,0xd2,0xeb,0x4b,0x8d,0x45,0xa8,0x68,
        0xea,0xf5,0xd3,0x49,0xe3,0xf8,0x44,0xf5,
        0xad,0xe7,0xd7,0x31,0x2e,0xfa,0xe1,0xd1,
        0x18,0x27,0x43,0x69,0x2c,0x9f,0xea,0x3d,
        0xc3,0x8f,0xf8,0x94,0x1d,0x53,0x48,0xe9,
        0x0a,0x33,0x59,0x90 };
    
    name = cipher_algor_table(kat->algor);
    
    err = ECC_Import_ANSI_X963(ecc414_pubkey, sizeof(ecc414_pubkey), &eccPub);CKERR;
    err = ECC_PubKeyHash(eccPub, keyID, kS4Key_KeyIDBytes, &keyIDLen);CKERR;
    err = S4Key_Import_ECC_Context(eccPub, &pubCtx); CKERR;
    eccPub = kInvalidECC_ContextRef;
    
    err = ECC_Import(ecc414_privkey, sizeof(ecc414_privkey), &eccPriv);CKERR;
    err = S4Key_Import_ECC_Context(eccPriv, &privCtx); CKERR;
    eccPriv = kInvalidECC_ContextRef;
   
    OPTESTLogVerbose("\t%-14s ", name);
    OPTESTLogVerbose("%6s", "Split");
    
    err = SHARES_Init( kat->key, kat->keysize >>3 ,
                      kNumShares,
                      kShareThreshold,
                      &ctx); CKERR;
    
    OPTESTLogVerbose("%8s", "Export");
    for(i = 0; i < kNumShares; i++)
    {
        size_t shareLen = 0;
        
        err = SHARES_GetShareInfo(ctx, i, &shares[i], &shareLen); CKERR;
        err = S4Key_NewShare( shares[i], &shareCtx[i]); CKERR;
        err = S4Key_SerializeToS4Key(shareCtx[i], pubCtx, &shareData[i], NULL); CKERR;
        
        OPTESTLogDebug("\n------\n%s",shareData[i]);
        
        if(shareData[i])
        {
            asprintf(&exported_keys,"%s %s %s", exported_keys,strlen(exported_keys) > 1?",":"", shareData[i] );
            exported_key_count++;
        }

    }
    
    OPTESTLogVerbose("%8s", "Decode");
    for(i = 0; i < kNumShares; i++)
    {
        S4KeyContextRef decodedCtx =  kInvalidS4KeyContextRef;
        
        err = S4Key_DeserializeKeys(shareData[i], strlen((char*)shareData[i]), &keyCount, &encodedCtx ); CKERR;
        ASSERTERR(keyCount == 1,  kS4Err_SelfTestFailed);
        
        err = S4Key_GetProperty(encodedCtx[0], kS4KeyProp_KeyID, NULL, keyID1, sizeof(keyID1), &keyIDLen1);
        ASSERTERR(keyIDLen == keyIDLen1,  kS4Err_SelfTestFailed);
        err = compareResults( keyID, keyID1, keyIDLen,
                             kResultFormat_Byte, "Pub KeyID"); CKERR;
        
        err = S4Key_DecryptFromS4Key(encodedCtx[0], privCtx, &decodedCtx); CKERR;

        err = sCompareKeys(decodedCtx, shareCtx[i], false); CKERR;
        
        recoveredShares[i] = XMALLOC(sizeof(SHARES_ShareInfo)); CKNULL(recoveredShares[i]);
        COPY(&decodedCtx->share, recoveredShares[i], sizeof(SHARES_ShareInfo));
        
        S4Key_Free(decodedCtx);
        
        S4Key_Free(encodedCtx[0]);
        XFREE(encodedCtx);
        encodedCtx = NULL;
    }
    
    OPTESTLogVerbose("%14s", "Reconstruct");
    err = SHARES_CombineShareInfo(kNumShares, recoveredShares, PT1, sizeof(PT1), &PT1len); CKERR;

    err = compare2Results( kat->key, kat->keysize >>3 ,PT1 ,PT1len, kResultFormat_Byte, "reconstructed key"); CKERR;
    
 done:
    
    if(encodedCtx)
    {
        if(S4KeyContextRefIsValid(encodedCtx[0]))
        {
            S4Key_Free(encodedCtx[0]);
        }
        XFREE(encodedCtx);
    }
    
    if(eccPub)
    {
        ECC_Free(eccPub);
        eccPub = kInvalidECC_ContextRef;
    }
    
    if(eccPriv)
    {
        ECC_Free(eccPriv);
        eccPriv = kInvalidECC_ContextRef;
    }
    if(pubCtx)
    {
        S4Key_Free(pubCtx);
    }
    if(privCtx)
    {
        S4Key_Free(privCtx);
    }

    
    for(i = 0; i < kNumShares; i++)
    {
        if(shares[i])
            XFREE(shares[i]);
        
        if(S4KeyContextRefIsValid(shareCtx[i]))
            S4Key_Free(shareCtx[i]);
        
        if(shareData[i])
            XFREE(shareData[i]);
        
        if(recoveredShares[i])
            XFREE(recoveredShares[i]);
        
    }
    
    if(SHARES_ContextRefIsValid(ctx))
        SHARES_Free(ctx);
    
    OPTESTLogVerbose("\n");
    
    return err;
}



static S4Err  sTest_SharedSymTBCKeys()
{
    S4Err     err = kS4Err_NoErr;
    int i;
    
    uint8_t* passPhrase1 = (uint8_t*)"Tant las fotei com auziretz";

    /* AES 128 bit key */
    uint8_t K1[] = {
        0x00, 0x01, 0x02, 0x03, 0x05, 0x06, 0x07, 0x08,
        0x0A, 0x0B, 0x0C, 0x0D, 0x0F, 0x10, 0x11, 0x12
    };
    
    /* AES 192 bit key */
    uint8_t K2[] = {
        0x00, 0x01, 0x02, 0x03, 0x05, 0x06, 0x07, 0x08,
        0x0A, 0x0B, 0x0C, 0x0D, 0x0F, 0x10, 0x11, 0x12,
        0x14, 0x15, 0x16, 0x17, 0x19, 0x1A, 0x1B, 0x1C
    };
    
    /* AES 256 bit key */
    uint8_t K3[] = {
        0x00, 0x01, 0x02, 0x03, 0x05, 0x06, 0x07, 0x08,
        0x0A, 0x0B, 0x0C, 0x0D, 0x0F, 0x10, 0x11, 0x12,
        0x14, 0x15, 0x16, 0x17, 0x19, 0x1A, 0x1B, 0x1C,
        0x1E, 0x1F, 0x20, 0x21, 0x23, 0x24, 0x25, 0x26
    };
 
    
    /* ThreeFish 256 bit key */
    uint64_t three_256_key[] = { 0x1716151413121110L, 0x1F1E1D1C1B1A1918L,
        0x2726252423222120L, 0x2F2E2D2C2B2A2928L
    };
    
    
    uint64_t three_512_key[] = { 0x1716151413121110L, 0x1F1E1D1C1B1A1918L,
        0x2726252423222120L, 0x2F2E2D2C2B2A2928L, 0x3736353433323130L,
        0x3F3E3D3C3B3A3938L, 0x4746454443424140L, 0x4F4E4D4C4B4A4948L
    };
    
    //
    //    uint64_t three_1024_key[] = { 0x1716151413121110L, 0x1F1E1D1C1B1A1918L,
    //        0x2726252423222120L, 0x2F2E2D2C2B2A2928L, 0x3736353433323130L,
    //        0x3F3E3D3C3B3A3938L, 0x4746454443424140L, 0x4F4E4D4C4B4A4948L,
    //        0x5756555453525150L, 0x5F5E5D5C5B5A5958L, 0x6766656463626160L,
    //        0x6F6E6D6C6B6A6968L, 0x7776757473727170L, 0x7F7E7D7C7B7A7978L,
    //        0x8786858483828180L, 0x8F8E8D8C8B8A8988L
    //    };
    
 
    cipherKATvector kat_vector_array[] =
    {
        {"Key 1",       kCipher_Algorithm_AES128, 128,	K1, passPhrase1},
        {"Key 2",       kCipher_Algorithm_AES192, 192,	K2, passPhrase1},
        {"Key 3",       kCipher_Algorithm_AES256, 256,   K3, passPhrase1},
        {"Key 4",       kCipher_Algorithm_2FISH256, 256,   K3, passPhrase1},
        {"TBC Key 256",	kCipher_Algorithm_3FISH256,   256,   (void*)  three_256_key , passPhrase1 },
        {"TBC Key 512",	kCipher_Algorithm_3FISH512,   512,   (void*)  three_512_key , passPhrase1 },
        
        // we dont support ECC encytion of the kCipher_Algorithm_3FISH1024 keys, Too big!
        
        //         {	kCipher_Algorithm_3FISH1024,  1024,  three_1024_key, NULL, NULL  },

    };

    OPTESTLogInfo("\nTesting Shared ECC Encrypted Symmetric and TBC S4Key Encoding\n");
      /* run  known answer tests (KAT) */
    for (i = 0; i < sizeof(kat_vector_array)/ sizeof(cipherKATvector) ; i++)
    {
        err = sRunSharedECCImportExportKAT( &kat_vector_array[i] ); CKERR;
    }

    OPTESTLogInfo("\nTesting Shared PBKDF2 Encrypted Symmetric and TBC S4Key Encoding\n");
     /* run  known answer tests (KAT) */
    for (i = 0; i < sizeof(kat_vector_array)/ sizeof(cipherKATvector) ; i++)
    {
        err = sRunSharedPBKDF2ImportExportKAT( &kat_vector_array[i] ); CKERR;
    }

done:
    return err;
}

static S4Err sRunTBCImportExportKAT(  cipherKATvector *kat)
{
    S4Err err = kS4Err_NoErr;
    S4KeyContextRef keyCtx =  kInvalidS4KeyContextRef;
    S4KeyContextRef keyCtx1=  kInvalidS4KeyContextRef;
    S4KeyContextRef passKeyCtx =  kInvalidS4KeyContextRef;

    S4KeyContextRef  *importCtx = NULL;
    
    uint8_t         unlockingKey[32];

    size_t      keyCount = 0;
    
	const char* name = NULL;

    uint8_t     *data = NULL;
    size_t      dataLen = 0;
    time_t          testDate  = time(NULL) ;
    
    name = cipher_algor_table(kat->algor);
    
    OPTESTLogVerbose("\t%-14s ", name);
    
    OPTESTLogVerbose("%8s", "Export");
    
    // create a random  unlocking key
    err = RNG_GetBytes(unlockingKey, sizeof(unlockingKey)); CKERR;
    
    err = S4Key_NewSymmetric(kCipher_Algorithm_2FISH256, unlockingKey, &passKeyCtx  ); CKERR;

    err = S4Key_NewTBC(kat->algor, kat->key, &keyCtx  ); CKERR;
    
    err = S4Key_SetProperty(keyCtx,kS4KeyProp_TestPassCodeID,S4KeyPropertyType_UTF8String, kat->comment, strlen(kat->comment)); CKERR;
    err = S4Key_SetProperty(keyCtx, kS4KeyProp_StartDate, S4KeyPropertyType_Time ,  &testDate, sizeof(time_t)); CKERR;
    
    err = S4Key_SerializeToS4Key(keyCtx, passKeyCtx, &data, &dataLen); CKERR;
    
    OPTESTLogDebug("\n------\n%s------\n",data);
    
    OPTESTLogVerbose("%8s", "Import");
    err = S4Key_DeserializeKeys(data, dataLen, &keyCount, &importCtx ); CKERR;
    ASSERTERR(keyCount == 1,  kS4Err_SelfTestFailed);
    
    OPTESTLogVerbose("%8s", "Verify");
    err = S4Key_DecryptFromS4Key(importCtx[0], passKeyCtx , &keyCtx1); CKERR;
    
    err = sCompareKeys(keyCtx, keyCtx1, false); CKERR;
    
    if(data)
    {
        asprintf(&exported_keys,"%s %s %s", exported_keys,strlen(exported_keys) > 1?",":"", data );
        exported_key_count++;
    }

done:
    
    if(data)
        XFREE(data);
    
    if(S4KeyContextRefIsValid(keyCtx))
    {
        S4Key_Free(keyCtx);
    }
    
    if(importCtx)
    {
        if(S4KeyContextRefIsValid(importCtx[0]))
        {
            S4Key_Free(importCtx[0]);
        }
        XFREE(importCtx);
        
    }
    
    if(S4KeyContextRefIsValid(passKeyCtx))
    {
        S4Key_Free(passKeyCtx);
    }
    
    if(S4KeyContextRefIsValid(keyCtx1))
    {
        S4Key_Free(keyCtx1);
    }
    
    
    OPTESTLogVerbose("\n");
    
    return err;

}

static S4Err sRunTBCPBKDF2ImportExportKAT(  cipherKATvector *kat)
{
    S4Err err = kS4Err_NoErr;
    S4KeyContextRef keyCtx =  kInvalidS4KeyContextRef;
    S4KeyContextRef keyCtx1=  kInvalidS4KeyContextRef;
    S4KeyContextRef  *passCtx = NULL;
    size_t      keyCount = 0;
 
	const char* name = NULL;

    uint8_t     *data = NULL;
    size_t      dataLen = 0;
     
    name = cipher_algor_table(kat->algor);
    
    OPTESTLogVerbose("\t%-14s ", name);
    
    OPTESTLogVerbose("%8s", "Export");
    
    err = S4Key_NewTBC(kat->algor, kat->key, &keyCtx  ); CKERR;
    
    err = S4Key_SetProperty(keyCtx,kS4KeyProp_TestPassCodeID,S4KeyPropertyType_UTF8String, kat->comment, strlen(kat->comment)); CKERR;
    err = S4Key_SerializeToPassPhrase(keyCtx, kat->passPhrase, strlen((char*)kat->passPhrase), &data, &dataLen); CKERR;
    
    OPTESTLogDebug("\n------\n%s------\n",data);

    OPTESTLogVerbose("%8s", "Import");
    err = S4Key_DeserializeKeys(data, dataLen, &keyCount, &passCtx ); CKERR;
    
  //  sDumpS4Key(OPTESTLOG_LEVEL_DEBUG, passCtx);

    OPTESTLogVerbose("%8s", "Verify");
    err = S4Key_VerifyPassPhrase(passCtx[0], kat->passPhrase, strlen((char*)kat->passPhrase)); CKERR;
    
    err = S4Key_DecryptFromPassPhrase(passCtx[0],kat->passPhrase, strlen((char*)kat->passPhrase), &keyCtx1); CKERR;
    
    err = sCompareKeys(keyCtx, keyCtx1, false); CKERR;
    
done:
    
    if(data)
        XFREE(data);

    if(S4KeyContextRefIsValid(keyCtx))
    {
        S4Key_Free(keyCtx);
    }
    if(passCtx)
    {
        if(S4KeyContextRefIsValid(passCtx[0]))
        {
            S4Key_Free(passCtx[0]);
        }
        XFREE(passCtx);
        
    }
    if(S4KeyContextRefIsValid(keyCtx1))
    {
        S4Key_Free(keyCtx1);
    }

    
    OPTESTLogVerbose("\n");
    
    return err;
}


static S4Err  sTestTBCKeys()
{
    S4Err     err = kS4Err_NoErr;
    int i;
    
    
    uint8_t* passPhrase1 =  (uint8_t*)"Tant las fotei com auziretz";
    
//    uint64_t three_256_tweak[] = { 0x0706050403020100L, 0x0F0E0D0C0B0A0908L };
 
    /* ThreeFish 256 bit key */
    uint64_t three_256_key[] = { 0x1716151413121110L, 0x1F1E1D1C1B1A1918L,
        0x2726252423222120L, 0x2F2E2D2C2B2A2928L
    };
 
    
    uint64_t three_512_key[] = { 0x1716151413121110L, 0x1F1E1D1C1B1A1918L,
        0x2726252423222120L, 0x2F2E2D2C2B2A2928L, 0x3736353433323130L,
        0x3F3E3D3C3B3A3938L, 0x4746454443424140L, 0x4F4E4D4C4B4A4948L
    };
    
 //   uint64_t three_512_tweak[] = { 0x0706050403020100L, 0x0F0E0D0C0B0A0908L };
    

    uint64_t three_1024_key[] = { 0x1716151413121110L, 0x1F1E1D1C1B1A1918L,
        0x2726252423222120L, 0x2F2E2D2C2B2A2928L, 0x3736353433323130L,
        0x3F3E3D3C3B3A3938L, 0x4746454443424140L, 0x4F4E4D4C4B4A4948L,
        0x5756555453525150L, 0x5F5E5D5C5B5A5958L, 0x6766656463626160L,
        0x6F6E6D6C6B6A6968L, 0x7776757473727170L, 0x7F7E7D7C7B7A7978L,
        0x8786858483828180L, 0x8F8E8D8C8B8A8988L
    };
//    uint64_t three_1024_tweak[] = { 0x0706050403020100L, 0x0F0E0D0C0B0A0908L };
    
    
    cipherKATvector kat_vector_array[] =
    {
        {"TBC Key 256", kCipher_Algorithm_3FISH256,   256,   (void*)  three_256_key , passPhrase1 },
        {"TBC Key 512", kCipher_Algorithm_3FISH512,   512,   (void*)  three_512_key , passPhrase1 },
        {"TBC Key 1K",	kCipher_Algorithm_3FISH1024,  1024,  (void*) three_1024_key  , passPhrase1  },
    };
    
    
    
    OPTESTLogInfo("\nTesting TBC S4Key Import / Export\n");
    
    /* run  known answer tests (KAT) */
    for (i = 0; i < sizeof(kat_vector_array)/ sizeof(cipherKATvector) ; i++)
    {
        err = sRunTBCImportExportKAT( &kat_vector_array[i] ); CKERR;
    }

    OPTESTLogInfo("\nTesting PBKDF2 TBC S4Key Import / Export\n");
    
    /* run  known answer tests (KAT) */
    for (i = 0; i < sizeof(kat_vector_array)/ sizeof(cipherKATvector) ; i++)
    {
        err = sRunTBCPBKDF2ImportExportKAT( &kat_vector_array[i] ); CKERR;
    }
    
    
done:
    return err;
}




static S4Err sRunCipherECCImportExportKAT(  cipherKATvector *kat)
{
    S4Err     err = kS4Err_NoErr;
    ECC_ContextRef eccPub = kInvalidECC_ContextRef;
    ECC_ContextRef eccPriv = kInvalidECC_ContextRef;
    S4KeyContextRef pubCtx =  kInvalidS4KeyContextRef;
    S4KeyContextRef privCtx=  kInvalidS4KeyContextRef;
  
    S4KeyContextRef keyCtx =  kInvalidS4KeyContextRef;
    S4KeyContextRef keyCtx1=  kInvalidS4KeyContextRef;
    S4KeyContextRef *encodedCtx =  NULL;
    size_t          keyCount = 0;
    
    uint8_t             keyID[kS4Key_KeyIDBytes]  = {0};
    size_t              keyIDLen = 0;
    
    uint8_t             keyID1[kS4Key_KeyIDBytes] = {0};
    size_t              keyIDLen1 = 0;

    uint8_t     *data = NULL;
    size_t      dataLen = 0;
	const char* name = NULL;

    uint8_t ecc414_pubkey[] = {
        0x04,0x06,0x8b,0x14,0xa4,0x14,0x6a,0x2a,
        0x3d,0xab,0x05,0xda,0xdf,0x75,0xef,0x5f,
        0xaf,0x7c,0xbf,0x8e,0x92,0x75,0x6c,0xe4,
        0x9f,0x93,0x69,0x6e,0x42,0x15,0x2e,0x9d,
        0xb2,0xde,0xd7,0xf0,0x79,0xbe,0xb6,0x12,
        0x1a,0x73,0x70,0x17,0x15,0x93,0x6f,0xa4,
        0x2c,0xbf,0x21,0x99,0xb6,0x23,0xa7,0xb7,
        0x0e,0x15,0x35,0x0d,0xf5,0x0e,0xc7,0xa0,
        0x2e,0xcf,0x66,0xac,0x65,0x3b,0x5c,0xf6,
        0x19,0xa1,0xdb,0x16,0x41,0x7f,0xef,0xb4,
        0x19,0x6a,0xd1,0xa4,0x91,0x4c,0x4e,0x6a,
        0x11,0xb6,0xfd,0xfa,0x90,0x11,0x13,0x10,
        0x0f,0x64,0xaf,0x65,0x0a,0x74,0x85,0x53,
        0x0d};
    
    
    uint8_t ecc414_privkey[] = {
        0x30,0x81,0xa9,0x03,0x02,0x07,0x80,0x02,
        0x01,0x34,0x02,0x34,0x06,0x8b,0x14,0xa4,
        0x14,0x6a,0x2a,0x3d,0xab,0x05,0xda,0xdf,
        0x75,0xef,0x5f,0xaf,0x7c,0xbf,0x8e,0x92,
        0x75,0x6c,0xe4,0x9f,0x93,0x69,0x6e,0x42,
        0x15,0x2e,0x9d,0xb2,0xde,0xd7,0xf0,0x79,
        0xbe,0xb6,0x12,0x1a,0x73,0x70,0x17,0x15,
        0x93,0x6f,0xa4,0x2c,0xbf,0x21,0x99,0xb6,
        0x02,0x34,0x23,0xa7,0xb7,0x0e,0x15,0x35,
        0x0d,0xf5,0x0e,0xc7,0xa0,0x2e,0xcf,0x66,
        0xac,0x65,0x3b,0x5c,0xf6,0x19,0xa1,0xdb,
        0x16,0x41,0x7f,0xef,0xb4,0x19,0x6a,0xd1,
        0xa4,0x91,0x4c,0x4e,0x6a,0x11,0xb6,0xfd,
        0xfa,0x90,0x11,0x13,0x10,0x0f,0x64,0xaf,
        0x65,0x0a,0x74,0x85,0x53,0x0d,0x02,0x34,
        0x2b,0x30,0xd2,0xe0,0x76,0xfd,0x09,0x6b,
        0xcc,0xd2,0xeb,0x4b,0x8d,0x45,0xa8,0x68,
        0xea,0xf5,0xd3,0x49,0xe3,0xf8,0x44,0xf5,
        0xad,0xe7,0xd7,0x31,0x2e,0xfa,0xe1,0xd1,
        0x18,0x27,0x43,0x69,0x2c,0x9f,0xea,0x3d,
        0xc3,0x8f,0xf8,0x94,0x1d,0x53,0x48,0xe9,
        0x0a,0x33,0x59,0x90 };
    
    name = cipher_algor_table(kat->algor);
    
    err = ECC_Import_ANSI_X963(ecc414_pubkey, sizeof(ecc414_pubkey), &eccPub);CKERR;
    err = ECC_PubKeyHash(eccPub, keyID, kS4Key_KeyIDBytes, &keyIDLen);CKERR;
    err = S4Key_Import_ECC_Context(eccPub, &pubCtx); CKERR;
    eccPub = kInvalidECC_ContextRef;

	err = ECC_Import(ecc414_privkey, sizeof(ecc414_privkey),&eccPriv);CKERR;
    err = S4Key_Import_ECC_Context(eccPriv, &privCtx); CKERR;
    eccPriv = kInvalidECC_ContextRef;

    OPTESTLogVerbose("\t%-14s ", name);
    OPTESTLogVerbose("%8s", "Export");

    err = S4Key_NewSymmetric(kat->algor, kat->key, &keyCtx  ); CKERR;
    
    err = S4Key_SerializeToS4Key(keyCtx, pubCtx, &data, &dataLen); CKERR;
    
    OPTESTLogDebug("\n------\n%s------\n",data);
    
    OPTESTLogVerbose("%8s", "Import");
    err = S4Key_DeserializeKeys(data, dataLen, &keyCount, &encodedCtx ); CKERR;
    
    err = S4Key_GetProperty(encodedCtx[0], kS4KeyProp_KeyID, NULL, keyID1, sizeof(keyID1), &keyIDLen1);
    ASSERTERR(keyIDLen == keyIDLen1,  kS4Err_SelfTestFailed);
    err = compareResults( keyID, keyID1, keyIDLen,
                         kResultFormat_Byte, "Pub KeyID"); CKERR;
    
    OPTESTLogVerbose("%8s", "Verify");
    err = S4Key_DecryptFromS4Key(encodedCtx[0], privCtx, &keyCtx1); CKERR;
   
    err = sCompareKeys(keyCtx, keyCtx1, false ); CKERR;

    if(data)
    {
        asprintf(&exported_keys,"%s %s %s", exported_keys,strlen(exported_keys) > 1?",":"", data );
        exported_key_count++;
    }
    
done:
    
    if(eccPub)
    {
        ECC_Free(eccPub);
        eccPub = kInvalidECC_ContextRef;
    }
    
    if(eccPriv)
    {
        ECC_Free(eccPriv);
        eccPriv = kInvalidECC_ContextRef;
    }
    
    if(pubCtx)
    {
        S4Key_Free(pubCtx);
    }
    if(privCtx)
    {
        S4Key_Free(privCtx);
    }

    if(keyCtx)
    {
        S4Key_Free(keyCtx);
    }
    if(keyCtx1)
    {
        S4Key_Free(keyCtx1);
    }
    
    if(encodedCtx)
    {
        if(S4KeyContextRefIsValid(encodedCtx[0]))
        {
            S4Key_Free(encodedCtx[0]);
        }
        XFREE(encodedCtx);
        
    }
    
     OPTESTLogVerbose("\n");

    return err;
}


static S4Err  sTestECC_SymmetricKeys()
{
    S4Err     err = kS4Err_NoErr;
    int i;
    
     /* AES 128 bit key */
    uint8_t K1[] = {
        0x00, 0x01, 0x02, 0x03, 0x05, 0x06, 0x07, 0x08,
        0x0A, 0x0B, 0x0C, 0x0D, 0x0F, 0x10, 0x11, 0x12
    };
    
    /* AES 192 bit key */
    uint8_t K2[] = {
        0x00, 0x01, 0x02, 0x03, 0x05, 0x06, 0x07, 0x08,
        0x0A, 0x0B, 0x0C, 0x0D, 0x0F, 0x10, 0x11, 0x12,
        0x14, 0x15, 0x16, 0x17, 0x19, 0x1A, 0x1B, 0x1C
    };
    
    /* AES 256 bit key */
    uint8_t K3[] = {
        0x00, 0x01, 0x02, 0x03, 0x05, 0x06, 0x07, 0x08,
        0x0A, 0x0B, 0x0C, 0x0D, 0x0F, 0x10, 0x11, 0x12,
        0x14, 0x15, 0x16, 0x17, 0x19, 0x1A, 0x1B, 0x1C,
        0x1E, 0x1F, 0x20, 0x21, 0x23, 0x24, 0x25, 0x26
    };
    
    cipherKATvector kat_vector_array[] =
    {
        {"Key 1",	kCipher_Algorithm_AES128, 128,	K1, NULL},
        {"Key 2",    kCipher_Algorithm_AES192, 192,	K2, NULL},
        {"Key 3",   kCipher_Algorithm_AES256, 256,   K3, NULL},
        {"Key 4",   kCipher_Algorithm_2FISH256, 256,   K3, NULL},
    };
    
    OPTESTLogInfo("\nTesting ECC Symmetric S4Key Encoding \n");
    
    /* run  known answer tests (KAT) */
    for (i = 0; i < sizeof(kat_vector_array)/ sizeof(cipherKATvector) ; i++)
    {
        err = sRunCipherECCImportExportKAT( &kat_vector_array[i] ); CKERR;
    }
    
    
done:
    return err;
}


static S4Err sRunTBC_ECCImportExportKAT(  cipherKATvector *kat)
{
    S4Err     err = kS4Err_NoErr;
    ECC_ContextRef eccPub = kInvalidECC_ContextRef;
    ECC_ContextRef eccPriv = kInvalidECC_ContextRef;
    S4KeyContextRef pubCtx =  kInvalidS4KeyContextRef;
    S4KeyContextRef privCtx=  kInvalidS4KeyContextRef;
    
    S4KeyContextRef keyCtx =  kInvalidS4KeyContextRef;
    S4KeyContextRef keyCtx1=  kInvalidS4KeyContextRef;
    S4KeyContextRef *encodedCtx =  NULL;
    size_t          keyCount = 0;
    
    uint8_t             keyID[kS4Key_KeyIDBytes]  = {0};
    size_t              keyIDLen = 0;
    
    uint8_t             keyID1[kS4Key_KeyIDBytes] = {0};
    size_t              keyIDLen1 = 0;

    uint8_t     *data = NULL;
    size_t      dataLen = 0;
	const char* name = NULL;

    
    uint8_t ecc414_pubkey[] = {
        0x04,0x06,0x8b,0x14,0xa4,0x14,0x6a,0x2a,
        0x3d,0xab,0x05,0xda,0xdf,0x75,0xef,0x5f,
        0xaf,0x7c,0xbf,0x8e,0x92,0x75,0x6c,0xe4,
        0x9f,0x93,0x69,0x6e,0x42,0x15,0x2e,0x9d,
        0xb2,0xde,0xd7,0xf0,0x79,0xbe,0xb6,0x12,
        0x1a,0x73,0x70,0x17,0x15,0x93,0x6f,0xa4,
        0x2c,0xbf,0x21,0x99,0xb6,0x23,0xa7,0xb7,
        0x0e,0x15,0x35,0x0d,0xf5,0x0e,0xc7,0xa0,
        0x2e,0xcf,0x66,0xac,0x65,0x3b,0x5c,0xf6,
        0x19,0xa1,0xdb,0x16,0x41,0x7f,0xef,0xb4,
        0x19,0x6a,0xd1,0xa4,0x91,0x4c,0x4e,0x6a,
        0x11,0xb6,0xfd,0xfa,0x90,0x11,0x13,0x10,
        0x0f,0x64,0xaf,0x65,0x0a,0x74,0x85,0x53,
        0x0d};
    
    
    uint8_t ecc414_privkey[] = {
        0x30,0x81,0xa9,0x03,0x02,0x07,0x80,0x02,
        0x01,0x34,0x02,0x34,0x06,0x8b,0x14,0xa4,
        0x14,0x6a,0x2a,0x3d,0xab,0x05,0xda,0xdf,
        0x75,0xef,0x5f,0xaf,0x7c,0xbf,0x8e,0x92,
        0x75,0x6c,0xe4,0x9f,0x93,0x69,0x6e,0x42,
        0x15,0x2e,0x9d,0xb2,0xde,0xd7,0xf0,0x79,
        0xbe,0xb6,0x12,0x1a,0x73,0x70,0x17,0x15,
        0x93,0x6f,0xa4,0x2c,0xbf,0x21,0x99,0xb6,
        0x02,0x34,0x23,0xa7,0xb7,0x0e,0x15,0x35,
        0x0d,0xf5,0x0e,0xc7,0xa0,0x2e,0xcf,0x66,
        0xac,0x65,0x3b,0x5c,0xf6,0x19,0xa1,0xdb,
        0x16,0x41,0x7f,0xef,0xb4,0x19,0x6a,0xd1,
        0xa4,0x91,0x4c,0x4e,0x6a,0x11,0xb6,0xfd,
        0xfa,0x90,0x11,0x13,0x10,0x0f,0x64,0xaf,
        0x65,0x0a,0x74,0x85,0x53,0x0d,0x02,0x34,
        0x2b,0x30,0xd2,0xe0,0x76,0xfd,0x09,0x6b,
        0xcc,0xd2,0xeb,0x4b,0x8d,0x45,0xa8,0x68,
        0xea,0xf5,0xd3,0x49,0xe3,0xf8,0x44,0xf5,
        0xad,0xe7,0xd7,0x31,0x2e,0xfa,0xe1,0xd1,
        0x18,0x27,0x43,0x69,0x2c,0x9f,0xea,0x3d,
        0xc3,0x8f,0xf8,0x94,0x1d,0x53,0x48,0xe9,
        0x0a,0x33,0x59,0x90 };
    
    
    name = cipher_algor_table(kat->algor);
    
   err = ECC_Import_ANSI_X963( ecc414_pubkey, sizeof(ecc414_pubkey),&eccPub);CKERR;
    err = ECC_PubKeyHash(eccPub, keyID, kS4Key_KeyIDBytes, &keyIDLen);CKERR;
    err = S4Key_Import_ECC_Context(eccPub, &pubCtx); CKERR;
    eccPub = kInvalidECC_ContextRef;
    
    err = ECC_Import(ecc414_privkey, sizeof(ecc414_privkey),&eccPriv);CKERR;
    err = S4Key_Import_ECC_Context(eccPriv, &privCtx); CKERR;
    eccPriv = kInvalidECC_ContextRef;
    
    OPTESTLogVerbose("\t%-14s ", name);
    OPTESTLogVerbose("%8s", "Export");
    
    err = S4Key_NewTBC(kat->algor, kat->key, &keyCtx  ); CKERR;
    
    err = S4Key_SerializeToS4Key(keyCtx, pubCtx, &data, &dataLen); CKERR;
    
    OPTESTLogDebug("\n------\n%s------\n",data);
    
    OPTESTLogVerbose("%8s", "Import");
    err = S4Key_DeserializeKeys(data, dataLen, &keyCount, &encodedCtx ); CKERR;
    
    err = S4Key_GetProperty(encodedCtx[0], kS4KeyProp_KeyID, NULL, keyID1, sizeof(keyID1), &keyIDLen1);
    ASSERTERR(keyIDLen == keyIDLen1,  kS4Err_SelfTestFailed);
    err = compareResults( keyID, keyID1, keyIDLen,
                         kResultFormat_Byte, "Pub KeyID"); CKERR;
    
    OPTESTLogVerbose("%8s", "Verify");
    err = S4Key_DecryptFromS4Key(encodedCtx[0], privCtx, &keyCtx1); CKERR;

    err = sCompareKeys(keyCtx, keyCtx1, false); CKERR;

    if(data)
    {
        asprintf(&exported_keys,"%s %s %s", exported_keys,strlen(exported_keys) > 1?",":"", data );
        exported_key_count++;
    }
    
    
done:
    if(eccPub)
    {
        ECC_Free(eccPub);
        eccPub = kInvalidECC_ContextRef;
    }
    
    if(eccPriv)
    {
        ECC_Free(eccPriv);
        eccPriv = kInvalidECC_ContextRef;
    }
   
    
    if(pubCtx)
    {
        S4Key_Free(pubCtx);
    }
    if(privCtx)
    {
        S4Key_Free(privCtx);
    }

    if(keyCtx)
    {
        S4Key_Free(keyCtx);
    }
    if(keyCtx1)
    {
        S4Key_Free(keyCtx1);
    }
    if(encodedCtx)
    {
        if(S4KeyContextRefIsValid(encodedCtx[0]))
        {
            S4Key_Free(encodedCtx[0]);
        }
        XFREE(encodedCtx);
        
    }
    
    OPTESTLogVerbose("\n");
    
    return err;
}



static S4Err  sTestECC_TBCKeys()
{
    S4Err     err = kS4Err_NoErr;
    int i;

    
    /* ThreeFish 256 bit key */
    uint64_t three_256_key[] = { 0x1716151413121110L, 0x1F1E1D1C1B1A1918L,
        0x2726252423222120L, 0x2F2E2D2C2B2A2928L
    };
    
    
    uint64_t three_512_key[] = { 0x1716151413121110L, 0x1F1E1D1C1B1A1918L,
        0x2726252423222120L, 0x2F2E2D2C2B2A2928L, 0x3736353433323130L,
        0x3F3E3D3C3B3A3938L, 0x4746454443424140L, 0x4F4E4D4C4B4A4948L
    };
    
//    
//    uint64_t three_1024_key[] = { 0x1716151413121110L, 0x1F1E1D1C1B1A1918L,
//        0x2726252423222120L, 0x2F2E2D2C2B2A2928L, 0x3736353433323130L,
//        0x3F3E3D3C3B3A3938L, 0x4746454443424140L, 0x4F4E4D4C4B4A4948L,
//        0x5756555453525150L, 0x5F5E5D5C5B5A5958L, 0x6766656463626160L,
//        0x6F6E6D6C6B6A6968L, 0x7776757473727170L, 0x7F7E7D7C7B7A7978L,
//        0x8786858483828180L, 0x8F8E8D8C8B8A8988L
//    };
    
    cipherKATvector kat_vector_array[] =
    {
        {"TBC Key 256",	kCipher_Algorithm_3FISH256,   256,     (void*)three_256_key, NULL },
        {"TBC Key 512",	kCipher_Algorithm_3FISH512,   512,     (void*)three_512_key, NULL },
        
// we dont support ECC encytion of the kCipher_Algorithm_3FISH1024 keys, Too big!
        
//         {	kCipher_Algorithm_3FISH1024,  1024,  three_1024_key, NULL, NULL  },
    };
    
    OPTESTLogInfo("\nTesting ECC TCC S4Key Import / Export\n");
    
    /* run  known answer tests (KAT) */
    for (i = 0; i < sizeof(kat_vector_array)/ sizeof(cipherKATvector) ; i++)
    {
        err = sRunTBC_ECCImportExportKAT( &kat_vector_array[i] ); CKERR;
    }
    
done:
    return err;

}

static S4Err sTestKeySigs( S4KeyContextRef pubCtx,
                          S4KeyContextRef otherKeyCtx)
{
    S4Err     err = kS4Err_NoErr;

    S4KeyContextRef     *sigListCtx = NULL;
    size_t              sigCount = 0;
 
    
    uint8_t             selfkeyID[kS4Key_KeyIDBytes]  = {0};
    uint8_t             otherKeyID[kS4Key_KeyIDBytes]  = {0};
   
    
    err = S4Key_GetProperty(pubCtx, kS4KeyProp_KeyID, NULL, &selfkeyID, sizeof(selfkeyID), NULL ); CKERR;
    err = S4Key_GetProperty(otherKeyCtx, kS4KeyProp_KeyID, NULL, &otherKeyID, sizeof(otherKeyID), NULL ); CKERR;
    
    // check sigs
    err =  S4Key_GetKeySignatures(pubCtx,&sigCount, &sigListCtx); CKERR;
    ASSERTERR(sigCount == 2,  kS4Err_SelfTestFailed);
    OPTESTLogDebug("\t  Found %d Key Signatures\n",sigCount );

    
    for(int i = 0; i <sigCount; i++)
    {
        S4KeyContextRef sigCtx = sigListCtx[i];
        if(sigCtx)
        {
            if(sigCtx->type == kS4KeyType_Signature)
            {
                OPTESTLogDebug("\t\t  KeyID: " );
                dumpKeyID(IF_LOG_DEBUG, sigCtx->sig.issuerID);
                OPTESTLogDebug(" " );
                dumpTime(IF_LOG_DEBUG, sigCtx->sig.signDate);
                if(sigCtx->sig.expirationTime)
                {
                    OPTESTLogDebug("  Exp: %ld", sigCtx->sig.expirationTime);
                }
                
                // check self sig
                if(S4Key_CompareKeyID(sigCtx->sig.issuerID, selfkeyID))
                {
                    err = S4Key_VerfiyKeySig(pubCtx, pubCtx, sigCtx); CKERR;
                }
                
                // check other sig
                else if(S4Key_CompareKeyID(sigCtx->sig.issuerID, otherKeyID))
                {
                    // check  sig
                    err = S4Key_VerfiyKeySig(pubCtx, otherKeyCtx, sigCtx); CKERR;
                }
                else
                {
                    OPTESTLogDebug("\n Found Other Key Sig: \n" );
                    ASSERTERR(0, kS4Err_SelfTestFailed);
                }
                
                OPTESTLogDebug(" - OK\n" );
                
            }
        }
    }

    
done:
   
    if(sigListCtx)
    {
        for(int i = 0; i <sigCount; i++)
            if(sigListCtx[i])  S4Key_Free(sigListCtx[i]);
  
        XFREE(sigListCtx);
    }
    
    return err;

}

static S4Err sRunPublicKeyTest( Cipher_Algorithm keyAlgorithm)
{
    S4Err     err = kS4Err_NoErr;
    S4KeyContextRef pubCtx      =  kInvalidS4KeyContextRef;
    S4KeyContextRef pubCtx1     =  kInvalidS4KeyContextRef;
    S4KeyContextRef signPubCtx1     =  kInvalidS4KeyContextRef;

    S4KeyContextRef passKeyCtx  =  kInvalidS4KeyContextRef;
    S4KeyContextRef copiedCtx   =  kInvalidS4KeyContextRef;
 
    S4KeyContextRef symKeyCtx   =  kInvalidS4KeyContextRef;
 
    S4KeyContextRef detSigCtx     =  kInvalidS4KeyContextRef;
    S4KeyContextRef detSigCtx1     =  kInvalidS4KeyContextRef;
    
    ECC_ContextRef  ecc         = kInvalidECC_ContextRef;
	const char* name = NULL;
    char* keyIDStr = NULL;
  
    uint8_t             keyID[kS4Key_KeyIDBytes]  = {0};
    uint8_t             keyID1[kS4Key_KeyIDBytes] = {0};
    
    uint8_t     unlockingKey[32];
    uint8_t     *data = NULL;
    size_t      dataLen = 0;
    time_t          testDate  = time(NULL) ;
  
    uint8_t        SIG[256];
    size_t         SIGlen = 0;

    S4KeyContextRef     *importCtx = NULL;  // typically an array of contexts
    S4KeyContextRef     importPubCtx =  NULL;  // just one key
    
    S4KeyPropertyExtendedType   exProp = S4KeyPropertyExtendedType_None;
    uint8_t K3[] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
    };

    size_t      keyCount = 0;
    
    name = cipher_algor_table(keyAlgorithm);
    
    OPTESTLogVerbose("\t%-8s\n", name);
    
    err = S4Key_NewPublicKey(keyAlgorithm, &signPubCtx1); CKERR;
    err = S4Key_SetPropertyExtended(signPubCtx1, kS4KeyProp_Owner, S4KeyPropertyType_UTF8String , S4KeyPropertyExtended_Signable,
                                    "Signing Guy", 11 ); CKERR;
    
    OPTESTLogDebug("\t  Create ");
    err = S4Key_NewPublicKey(keyAlgorithm, &pubCtx); CKERR;
  
    
    err = S4Key_SetProperty(pubCtx, kS4KeyProp_StartDate, S4KeyPropertyType_Time,
                                    &testDate, sizeof(time_t)); CKERR;
    
    // check that kS4KeyProp_StartDate shows asS4KeyPropertyExtended_Signable
    err = S4Key_GetExtendedProperty(pubCtx, kS4KeyProp_StartDate, &exProp); CKERR;
    ASSERTERR(exProp && S4KeyPropertyExtended_Signable == S4KeyPropertyExtended_Signable,  kS4Err_SelfTestFailed);

    err = S4Key_SetPropertyExtended(pubCtx, kS4KeyProp_Owner, S4KeyPropertyType_UTF8String , S4KeyPropertyExtended_Signable,
                                    "PhucLong", 8 ); CKERR;
  
    err = S4Key_GetExtendedProperty(pubCtx, kS4KeyProp_Owner, &exProp); CKERR;
    ASSERTERR(exProp && S4KeyPropertyExtended_Signable == S4KeyPropertyExtended_Signable,  kS4Err_SelfTestFailed);

    err = S4Key_GetAllocatedProperty(pubCtx, kS4KeyProp_KeyIDString, NULL, (void**)&keyIDStr, NULL); CKERR;
    OPTESTLogDebug("KeyID: %s\n",  keyIDStr);

    // test add / delete property
    err = S4Key_SetPropertyExtended(pubCtx, "delete", S4KeyPropertyType_UTF8String , S4KeyPropertyExtended_Signable,
                                    "12345678", 8 ); CKERR;
   
    ASSERTERR( S4Key_GetProperty(pubCtx, "delete", NULL, NULL, 0, &dataLen) == kS4Err_NoErr, kS4Err_SelfTestFailed);
    ASSERTERR(dataLen == 8, kS4Err_SelfTestFailed);
    
    err = S4Key_RemoveProperty(pubCtx, "delete"); CKERR
    
     ASSERTERR(S4Key_GetProperty(pubCtx, "delete", NULL, NULL, 0, NULL) == kS4Err_PropertyNotFound, kS4Err_SelfTestFailed);
    
    // test hash signing
    OPTESTLogDebug("\t  Low level Public Key Sign / Verify\n");
    err = S4Key_SignHash(pubCtx, K3,sizeof(K3),SIG, sizeof(SIG), &SIGlen); CKERR;
  
    OPTESTLogDebug("\t\tPublic Key Sign - low level (%ld bytes) \n", SIGlen);
    err = S4Key_VerifyHash(pubCtx,  K3,sizeof(K3), SIG, SIGlen);  CKERR;
    
    OPTESTLogDebug("\t  additional sign key\n");
    err = S4Key_SignKey(signPubCtx1, pubCtx, 30 * 60*60*24 ); CKERR;
   
     // check cloning
    err = S4Key_Copy(pubCtx, &copiedCtx);
    err = sCompareKeys(pubCtx,copiedCtx, true); CKERR;
    S4Key_Free(copiedCtx); copiedCtx = kInvalidS4KeyContextRef;
    
    OPTESTLogDebug("\t  Import/Export Pub\n");
    err = S4Key_SerializePubKey(pubCtx, &data, &dataLen); CKERR;
    OPTESTLogDebug("------\n%s------\n",data);
   
    err = S4Key_DeserializeKey(data, dataLen, &importPubCtx ); CKERR;
     XFREE(data); data = NULL;
    err = sCompareKeys(pubCtx, importPubCtx, true); CKERR;
    
    // check in imported properties show as signable
    err = S4Key_GetExtendedProperty(importPubCtx, kS4KeyProp_Owner, &exProp); CKERR;
    ASSERTERR(exProp && S4KeyPropertyExtended_Signable == S4KeyPropertyExtended_Signable,  kS4Err_SelfTestFailed);

     err = S4Key_GetExtendedProperty(importPubCtx, kS4KeyProp_StartDate, &exProp); CKERR;
     ASSERTERR(exProp && S4KeyPropertyExtended_Signable == S4KeyPropertyExtended_Signable,  kS4Err_SelfTestFailed);

    // check sigs
    err = sTestKeySigs(importPubCtx,signPubCtx1); CKERR;
    
    // check the key itself
    err = S4Key_Clone_ECC_Context(importPubCtx, &ecc);
    ASSERTERR(!ECC_isPrivate(ecc),  kS4Err_SelfTestFailed);
    err = ECC_PubKeyHash(ecc, keyID, kS4Key_KeyIDBytes, NULL);CKERR;
    err = S4Key_GetProperty(importPubCtx, kS4KeyProp_KeyID, NULL, &keyID1, sizeof(keyID1), NULL ); CKERR;
    err = compareResults( keyID, keyID1, kS4Key_KeyIDBytes, kResultFormat_Byte, "keyID"); CKERR;
    ECC_Free(ecc); ecc  = kInvalidECC_ContextRef;
    
    OPTESTLogDebug("\t  Encrypt to Public Key\n");
    // encrypt to public key
    err = S4Key_NewSymmetric(kCipher_Algorithm_2FISH256, K3, &symKeyCtx  ); CKERR;
    err = S4Key_SerializeToS4Key(symKeyCtx, importPubCtx, &data, &dataLen); CKERR;
    OPTESTLogDebug("------\n%s------\n",data);
    XFREE(data); data = NULL;
    
    OPTESTLogDebug("\t  Clone Public Key\n");
    err = S4Key_Copy(importPubCtx, &copiedCtx);
    err = sCompareKeys(importPubCtx,copiedCtx, true); CKERR;
    S4Key_Free(copiedCtx); copiedCtx = kInvalidS4KeyContextRef;
    
    // create a random  unlocking key
    err = RNG_GetBytes(unlockingKey, sizeof(unlockingKey)); CKERR;
    err = S4Key_NewSymmetric(kCipher_Algorithm_2FISH256, unlockingKey, &passKeyCtx  ); CKERR;

    OPTESTLogDebug("\t  Import/Export Private\n");
    err = S4Key_SerializeToS4Key(pubCtx, passKeyCtx, &data, &dataLen); CKERR;
   OPTESTLogDebug("\n------\n%s------\n",data);
   
    err = S4Key_DeserializeKeys(data, dataLen, &keyCount, &importCtx ); CKERR;
    ASSERTERR(keyCount == 1,  kS4Err_SelfTestFailed);
    XFREE(data); data = NULL;
    
    err = S4Key_DecryptFromS4Key(importCtx[0], passKeyCtx , &pubCtx1); CKERR;
    if(importPubCtx)
    {
        if(S4KeyContextRefIsValid(importPubCtx))
        {
            S4Key_Free(importPubCtx);
        }
        importPubCtx = NULL;
    }
    
    err = sCompareKeys(pubCtx, pubCtx1, true); CKERR;
   
    // check sigs
    err = sTestKeySigs(pubCtx1,signPubCtx1);CKERR;
    
    // check the ecc key itself
    err = S4Key_Clone_ECC_Context(pubCtx1, &ecc);
    ASSERTERR(ECC_isPrivate(ecc),  kS4Err_SelfTestFailed);
    err = ECC_PubKeyHash(ecc, keyID, kS4Key_KeyIDBytes, NULL);CKERR;
    err = S4Key_GetProperty(pubCtx1, kS4KeyProp_KeyID, NULL, &keyID1, sizeof(keyID1), NULL ); CKERR;
    err = compareResults( keyID, keyID1, kS4Key_KeyIDBytes, kResultFormat_Byte, "keyID"); CKERR;
    ECC_Free(ecc); ecc  = kInvalidECC_ContextRef;

    OPTESTLogDebug("\t  Clone Private\n");
    err = S4Key_Copy(pubCtx1, &copiedCtx);
    err = sCompareKeys(pubCtx1,copiedCtx, true); CKERR;
    S4Key_Free(copiedCtx); copiedCtx = kInvalidS4KeyContextRef;

    OPTESTLogDebug("\t  Test Detached Sigs\n");
    err = S4Key_NewSignature(signPubCtx1, K3,sizeof(K3), kHASH_Algorithm_SHA256,
                             30 * 60*60*24,  &detSigCtx); CKERR;
    err = S4Key_SetProperty(detSigCtx, "FileName", S4KeyPropertyType_UTF8String , "foo.bar", 7 ); CKERR;

    uint8_t     certKeyID[kS4Key_KeyIDBytes]  = {0};
    
    err = S4Key_SignKey(pubCtx, detSigCtx, 300); CKERR;
    err = S4Key_GetProperty(pubCtx, kS4KeyProp_KeyID, NULL, &certKeyID, sizeof(certKeyID), NULL ); CKERR;
    
    err = S4Key_Copy(detSigCtx, &detSigCtx1); CKERR;
    err = sCompareKeys(detSigCtx, detSigCtx1, true); CKERR;
    
    err = S4Key_SerializeSignature(detSigCtx1, &data, &dataLen); CKERR;
    OPTESTLogDebug("------\n%s------\n",data);
  
    err = S4Key_DeserializeKeys(data, dataLen, &keyCount, &importCtx ); CKERR;
    ASSERTERR(keyCount == 1,  kS4Err_SelfTestFailed);
    XFREE(data); data = NULL;
    
    err = sCompareKeys(detSigCtx, importCtx[0], true); CKERR;
   
    // check for proper signing key

    err = S4Key_GetProperty(importCtx[0], kS4KeyProp_SignedBy, NULL, &keyID, sizeof(keyID), NULL ); CKERR;
    err = S4Key_GetProperty(signPubCtx1, kS4KeyProp_KeyID, NULL, &keyID1, sizeof(keyID), NULL ); CKERR;
    if( S4Key_CompareKeyID(keyID1, keyID))
    {
        err = S4Key_VerifySignature(importCtx[0], signPubCtx1, K3,sizeof(K3));CKERR;
    }
    else
    {
        OPTESTLogDebug("\n Found Other Key Sig: \n" );
        ASSERTERR(0, kS4Err_SelfTestFailed);
    }
    
    // check sub sigs
    {
        S4KeyContextRef     *sigListCtx = NULL;
        size_t              sigCount = 0;
        
        // check sigs
        err =  S4Key_GetKeySignatures(importCtx[0],&sigCount, &sigListCtx); CKERR;
        ASSERTERR(sigCount == 1,  kS4Err_SelfTestFailed);
        OPTESTLogDebug("\t  Found %d Key Signatures\n",sigCount );

        for(int i = 0; i <sigCount; i++)
        {
            S4KeyContextRef sigCtx = sigListCtx[i];
            if(sigCtx)
            {
                if(sigCtx->type == kS4KeyType_Signature)
                {
                    OPTESTLogDebug("\t\t  KeyID: " );
                    dumpKeyID(IF_LOG_DEBUG, sigCtx->sig.issuerID);
                    OPTESTLogDebug(" " );
                    dumpTime(IF_LOG_DEBUG, sigCtx->sig.signDate);
                    if(sigCtx->sig.expirationTime)
                    {
                        OPTESTLogDebug("  Exp: %ld", sigCtx->sig.expirationTime);
                    }
                    
                    if(S4Key_CompareKeyID(sigCtx->sig.issuerID, certKeyID))
                    {
                        // check  sig
                        err = S4Key_VerfiyKeySig(importCtx[0], pubCtx, sigCtx); CKERR;
                    }
                    else
                    {
                        OPTESTLogDebug("\n Found Other Key Sig: \n" );
                        ASSERTERR(0, kS4Err_SelfTestFailed);
                    }
                    OPTESTLogDebug(" - OK\n" );
                    
                }
            }
        }
        if(sigListCtx)
        {
            for(int i = 0; i <sigCount; i++)
                if(sigListCtx[i])  S4Key_Free(sigListCtx[i]);
            
            XFREE(sigListCtx);
        }

    }
    
   done:
    if(data)
        XFREE(data);
    
    if(ecc)
    ECC_Free(ecc);
  
    if(importPubCtx)
    {
        if(S4KeyContextRefIsValid(importPubCtx))
        {
            S4Key_Free(importPubCtx);
        }
	}
    
    if(S4KeyContextRefIsValid(copiedCtx))
    {
        S4Key_Free(copiedCtx);
    }

    if(S4KeyContextRefIsValid(detSigCtx))
    {
        S4Key_Free(detSigCtx);
    }
 
    if(S4KeyContextRefIsValid(detSigCtx1))
    {
        S4Key_Free(detSigCtx1);
    }

    if(S4KeyContextRefIsValid(pubCtx))
    {
        S4Key_Free(pubCtx);
    }
    
    if(S4KeyContextRefIsValid(pubCtx1))
    {
        S4Key_Free(pubCtx1);
    }
    
    if(S4KeyContextRefIsValid(passKeyCtx))
    {
        S4Key_Free(passKeyCtx);
    }
   
    if(S4KeyContextRefIsValid(signPubCtx1))
    {
        S4Key_Free(signPubCtx1);
    }
   
    OPTESTLogDebug("\n");
    return err;
}


static S4Err  sTestPublicKeys()
{
    S4Err     err = kS4Err_NoErr;
    
    OPTESTLogInfo("\nTesting Public Key API\n");
    
    err = sRunPublicKeyTest(kCipher_Algorithm_ECC384);  CKERR;
    err = sRunPublicKeyTest(kCipher_Algorithm_ECC414);  CKERR;
    
    
done:
    return err;
    
}

S4Err  TestKeys()
{
    S4Err     err = kS4Err_NoErr;
    S4KeyContextRef *encodedCtx =  NULL;
    size_t          keyCount = 0;
    int i;
    
    asprintf(&exported_keys,"[" );

    err = sTestSymmetricKeys(); CKERR;
    err = sTestTBCKeys(); CKERR;
    err = sTestECC_TBCKeys(); CKERR;
    err = sTestECC_SymmetricKeys(); CKERR;
    err = sTest_SharedSymTBCKeys(); CKERR;
    err = sTestPublicKeys(); CKERR;

    OPTESTLogInfo("\nTesting decoding of exported key array\n");
    asprintf(&exported_keys,"%s ]", exported_keys );
    err = S4Key_DeserializeKeys((uint8_t*)exported_keys, strlen(exported_keys), &keyCount, &encodedCtx ); CKERR;
    OPTESTLogInfo("\tDecoded %d Items\n", keyCount);
    ASSERTERR(keyCount == exported_key_count, kS4Err_SelfTestFailed);
    
    for(i = 0; i < keyCount; i++)
    {
        if(S4KeyContextRefIsValid(encodedCtx[i]))
        {
            S4KeyContextRef keyP = encodedCtx[i];
            
            S4KeyType   type1;
            char**       keyIDStr = NULL;
			char**       p2kParamStr = NULL;
            size_t      datSize = 0;
            
            err = S4Key_GetProperty(keyP, kS4KeyProp_KeyType, NULL, &type1, sizeof(type1), NULL ); CKERR;
            switch (type1)
            {
                case kS4KeyType_PBKDF2:
                    err = S4Key_GetAllocatedProperty(keyP, kS4KeyProp_TestPassCodeID, NULL, (void**)&keyIDStr, &datSize);
                    OPTESTLogDebug("\t%2d %10s %.*s\n", i,  key_type_table(type1), datSize,keyIDStr);
                    break;

				case kS4KeyType_P2K:
					err = S4Key_GetAllocatedProperty(keyP, kS4KeyProp_TestPassCodeID, NULL, (void**)&keyIDStr, &datSize); CKERR;
					OPTESTLogDebug("\t%2d %10s %.*s\n", i,  key_type_table(type1), datSize,keyIDStr);

					err = S4Key_GetAllocatedProperty(keyP, kS4KeyProp_p2kParams, NULL, (void**)&p2kParamStr, &datSize); CKERR;
					OPTESTLogDebug("\t%2d %10s %.*s\n", i,  key_type_table(type1), datSize,p2kParamStr);


					break;

                case kS4KeyType_PublicEncrypted:
                     err = S4Key_GetAllocatedProperty(keyP, kS4KeyProp_KeyIDString, NULL, (void**)&keyIDStr, &datSize); CKERR;
                    OPTESTLogDebug("\t%2d %10s %.*s\n", i,  key_type_table(type1),datSize, keyIDStr);
                    
                break;
                    
                default:
                    break;
            }

			if(p2kParamStr) XFREE(p2kParamStr);
            if(keyIDStr) XFREE(keyIDStr);
            
        }
    }
    
    OPTESTLogInfo("\n");

    
done:
    
    if(encodedCtx)
    {
       
        for(i = 0; i < keyCount; i++)
            if(S4KeyContextRefIsValid(encodedCtx[i]))
            {
                S4Key_Free(encodedCtx[i]);
            }
        XFREE(encodedCtx);
    }
 
    if(exported_keys)
        free(exported_keys);
    
    return err;
    
}
