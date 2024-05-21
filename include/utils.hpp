#ifndef ACONN__UTILS__
#define ACONN__UTILS__

int my_gets(char *str,int num){
	int cnt=0;
	char x=getchar();
	while(x!='\n'&&cnt<num-1){
		str[cnt++]=x;
		x=getchar();
	}
	str[cnt++]='\0';
	return cnt;
}

void printf_hex(const void *s,int num){
	const unsigned char *c=(const unsigned char *)s;
	for(int i=0;i<num;i++){
		printf("%x",c[i]);
	}
	printf("\n");
}

#endif // ACONN__UTILS__