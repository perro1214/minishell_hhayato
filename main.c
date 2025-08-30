#include "lexer_parser.h"

int	main(int argc, char **argv, char **envp)
{
	char	*input;
	t_token	*tokens;
	t_ast	*ast;
	size_t	len;
	t_data	data;

	(void)argc;
	(void)argv;
	
	if (init_env_list(&data, envp) == -1)
	{
		printf("環境変数の初期化に失敗しました\n");
		return (1);
	}
	
	printf("Lexer & Parser Test Program with Variable Expansion\n");
	printf("Enter a command (or press Ctrl+D to exit):\n");
	
	input = NULL;
	len = 0;
	while (getline(&input, &len, stdin) != -1)
	{
		if (input[strlen(input) - 1] == '\n')
			input[strlen(input) - 1] = '\0';
		
		if (strlen(input) == 0)
		{
			printf("Enter a command (or press Ctrl+D to exit):\n");
			continue;
		}
		
		printf("\n--- Input: %s ---\n", input);
		
		tokens = tokenize(input);
		if (!tokens)
		{
			printf("Tokenization failed!\n");
			printf("Enter a command (or press Ctrl+D to exit):\n");
			continue;
		}
		
		printf("\n=== TOKENS ===\n");
		print_tokens(tokens);
		
		ast = parse(tokens);
		if (!ast)
		{
			printf("Parsing failed!\n");
		}
		else
		{
			t_command_invocation *cmd;
			
			printf("\n=== AST ===\n");
			print_ast(ast, 0);
			
			cmd = ast_to_command_invocation(ast, data.env_head);
			if (!cmd)
			{
				printf("AST to command_invocation conversion failed!\n");
			}
			else
			{
				printf("\n=== 🚀 コマンド実行構造体 (変数展開済み) ===\n");
				print_command_invocation(cmd, 0);
				
				printf("\n=== 実行結果 ===\n");
				fflush(stdout);
				execute_pipeline(cmd, data.env_head);
				
				free_command_invocation(cmd);
			}
			
			free_ast(ast);
		}
		
		free_tokens(tokens);
		printf("\nEnter a command (or press Ctrl+D to exit):\n");
	}
	
	free(input);
	free_env_list(data.env_head);
	printf("\nGoodbye!\n");
	return (0);
}