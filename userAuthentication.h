#define TOKEN_LEN 16

char *sessionToken;

namespace userAuthentication {

char *generateToken() {
  // randomize sequence for tokens becuase they should not be guessable!
  randomSeed(millis());

  char *token = new char[TOKEN_LEN + 1];
  int randMax = 10;
  int n;

  for (int i = 0; i < TOKEN_LEN; i++) {
    // convert random numbers to their char form using ASCII code
    token[i] = ( (char) (random(0, randMax) + '0') );
  }

  token[TOKEN_LEN] = '\0';
  return token;
}

char *initSession () {
  if (!sessionToken || sessionToken[0] == '\0') {
    delete [] sessionToken;
  }
  sessionToken = generateToken();
  return sessionToken;
}

bool isTokenMatch (char *token) {
  if (!token || token[0] == '\0') {
    return false;
  }
  return (strcmp(token, sessionToken) == 0);
}

}

