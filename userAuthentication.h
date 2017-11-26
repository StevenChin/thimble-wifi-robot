#include <sha256.h>

#define TOKEN_LEN 16
#define HASHED_PWD_LEN 64

namespace userAuthentication {

struct Credential {
  char *username;
  char *pwd;
  char *token;
};

Credential creds = {
  "<username>",
  "<hashed pwd>"
};

const char alphanumeric_table[62] = {
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
  'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
  'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'
};

void printHash(uint8_t *hash) {
  for (int i = 0; i < 32; i++) {
    Serial.print("0123456789abcdef"[hash[i] >> 4]);
    Serial.print("0123456789abcdef"[hash[i] & 0xf]);
  }

  Serial.println();
}

void convertHashToString(uint8_t *hash, char *hashStr) {
  int hashIndex = 0;

  for (int i = 0; i < 32; i++) {
    hashStr[hashIndex] = "0123456789abcdef"[hash[i] >> 4];
    hashIndex++;
    hashStr[hashIndex] = "0123456789abcdef"[hash[i] & 0xf];
    hashIndex++;
  }

  // null terminate array
  hashStr[hashIndex] = '\0';
}

void hashPassword(char *pwd,  char *hashedPwd) {
  uint8_t *hash;
  Sha256 sha256;
  sha256.init();
  sha256.print(pwd);
  hash = sha256.result();
  convertHashToString(hash, hashedPwd);
}

char *generateToken() {
  // randomize sequence for tokens becuase they should not be guessable!
  randomSeed(millis());

  char *token = new char[TOKEN_LEN + 1];
  int randMax = sizeof(alphanumeric_table);

  for (int i = 0; i < TOKEN_LEN; i++) {
    token[i] = alphanumeric_table[random(0, randMax)];
  }

  token[TOKEN_LEN] = '\0';
  return token;
}

char *authenticate (char *username, char *pwd) {
  char *token;
  char *hashedPwd = new char[HASHED_PWD_LEN + 1];
  hashPassword(pwd, hashedPwd);

  if ( strcmp(username, creds.username) == 0 && strcmp(hashedPwd, creds.pwd) == 0) {
    delete [] creds.token;
    creds.token = generateToken();
    token = creds.token;
  } else {
    delete [] token;
  }

  delete [] hashedPwd;
  return token;
}

bool isTokenMatch (char *token) {
  return strcmp(token, creds.token) == 0;
}

}

