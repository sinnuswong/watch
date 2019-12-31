# watch
https://www.cairographics.org/samples/
https://developer.tizen.org/development/guides/native-application/graphics/cairo-vector-graphics
https://developer.tizen.org/development/sample/native/AppFW/Simple_home
https://www.cairographics.org/manual/cairo-text.html#cairo-toy-font-face-get-family

https://www.cairographics.org/manual/cairo-text.html#cairo-get-font-face
https://apps.apple.com/cn/app/ecg-%E6%B5%B7%E9%87%8F%E5%BF%83%E7%94%B5%E5%9B%BE%E6%A1%88%E4%BE%8B%E5%92%8C%E8%AF%8A%E6%96%AD%E8%AF%A6%E8%A7%A3/id798663024#?platform=appleWatch
int main()
{
	char* s = "8*7";
	char* str = (char*)malloc(128*sizeof(char));
	char* res = (char*)malloc(128 * sizeof(char));;
	strcpy(str, s);
	token* tokens = (token*) malloc(sizeof(token));
	int numTokens = 0;
	Stack expr;
	int i;
	int ch, rflag = 0;
	prefs.precision = DEFAULTPRECISION;
	prefs.maxtokenlength = MAXTOKENLENGTH;


	if(str != NULL && strcmp(str, "quit") != 0)
	{
		
		if(type(*str) == text)
		{
			// Do something with command
			if (!execCommand(str)) {
				//printf("!execCommand(str)");
				goto no_command;
			}
			else {
				//printf("execCommand(str)");
			}
			free(str);
			str = NULL;
		}
		else
		{
no_command:
			numTokens = tokenize(str, &tokens);
			free(str);
			str = NULL;

			if(prefs.display.tokens)
			{
				printf("\t%d tokens:\n", numTokens);
				for(i = 0; i < numTokens; i++)
				{
					printf("\t\"%s\"", tokens[i]);
					if(tokenType(tokens[i]) == value)
						printf("result = %f", buildNumber(tokens[i]));
					printf("\n");
				}
			}

			// Convert to postfix
			stackInit(&expr, numTokens);
			if(prefs.display.postfix)
				printf("\tPostfix stack:\n");
			postfix(tokens, numTokens, &expr);
			//stackReverse(&expr);
			/*printf("\tReversed postfix stack:\n\t");
			for(i = 0; i < stackSize(&expr); i++)
			{
				printf("%s ", (token)(expr.content[i]));
			}
			printf("\n");*/
			if(stackSize(&expr) != 1)
			{
				printf("\tError evaluating expression\n");
			}
			else
			{
				
				//printf("%s\n", (char*)stackTop(&expr));
				strcpy(res, (char*)stackTop(&expr));
				for (i=0; i< numTokens; i++)
				{
					if (tokens[i] == stackTop(&expr))
						tokens[i] = NULL;
				}
				free(stackPop(&expr));
			}

			for(i = 0; i < numTokens; i++)
			{
				if (tokens[i] != NULL)
					free(tokens[i]);
			}
			free(tokens);
			tokens = NULL;
			numTokens = 0;
			stackFree(&expr);
		}

	}

	free(str);
	str = NULL;


	printf("%s\n", res);
	return EXIT_SUCCESS;
}
