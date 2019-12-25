# watch
https://www.cairographics.org/manual/cairo-text.html#cairo-get-font-face
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
