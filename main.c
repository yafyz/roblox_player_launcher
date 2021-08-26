#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <Windows.h>

int main(int argc, char* argv[]) {
    char* launchstr = argv[1];
    char ch;

    int larg_len = 0;
    int larg_s = 0;
    int v_split_off = 0;
    int v_len;
    int urlenc_count = 0;

    char* authticket;
    char* placelauncher;

    for (int i = 0; i < strlen(launchstr); i++) {
        ch = launchstr[i];
        larg_len++;

        if (ch == '+') {
            //printf("%.*s %.*s\n", v_split_off-1, launchstr+larg_s, larg_len-v_split_off-1, launchstr+larg_s+v_split_off);
            if (strncmp("gameinfo", launchstr+larg_s, v_split_off-1) == 0) {
                v_len = larg_len-v_split_off-1;
                authticket = (char*) malloc(v_len+1);
                memcpy(authticket, launchstr+larg_s+v_split_off, v_len);
                authticket[v_len] = 0;
            } else if (strncmp("placelauncherurl", launchstr+larg_s, v_split_off-1) == 0) {
                v_len = larg_len-v_split_off-1;
                placelauncher = (char*) malloc(v_len-urlenc_count*2+1);
                int len = decode(launchstr+larg_s+v_split_off, placelauncher, v_len);
                placelauncher[len] = 0;
            }
            larg_s = i+1;
            larg_len = 0;
            v_split_off = 0;
            urlenc_count = 0;
        } else if (ch == ':') {
            v_split_off = larg_len;
        } else if (ch == '%') {
            urlenc_count++;
        }
    }

    char rbxpath[FILENAME_MAX];
    int last_slash = 0;

    GetModuleFileName(NULL, rbxpath, FILENAME_MAX);
    for (int i = 0; i < strlen(rbxpath); i++)
        if (rbxpath[i] == '\\' || rbxpath[i] == '/')
            last_slash = i;
    rbxpath[last_slash+1] = 0;
    sprintf(rbxpath, "%sRobloxPlayerBeta.exe", rbxpath);

    char* args[] = {"--play", "-t", authticket, "-j", placelauncher, "--rloc", "en_us", "--gloc", "en_us", NULL};
    int ret = execv(rbxpath, args);

    if (ret < 0) {
        printf("%s\n", strerror(errno));
        getch();
    }
    return 0;
}

inline int ishex(int x) { // not feeling like writing a url decode rn https://www.rosettacode.org/wiki/URL_decoding#C
	return (x >= '0' && x <= '9') ||
		   (x >= 'a' && x <= 'f') ||
		   (x >= 'A' && x <= 'F');
}

int decode(char *s, char *dec, int len) {
	char* o;
	const char* end = s + len;
	int c;

	for (o = dec; s <= end; o++) {
		c = *s++;
		if (c == '+')
            c = ' ';
		else if (c == '%' && (!ishex(*s++) || !ishex(*s++) || !sscanf(s - 2, "%2x", &c)))
			return -1;
		if (dec)
            *o = c;
	}
	return o - dec;
}