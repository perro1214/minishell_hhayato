 % norminette ./*.c                                                             
Setting locale to en_US
command_args.c: OK!
command_execution.c: OK!
command_free.c: OK!
command_process_redir.c: OK!
command_redirection.c: OK!
expander.c: OK!
expander_calc.c: Error!
Error: MIXED_SPACE_TAB      (line:  60, col:  48):      Mixed spaces and tabs
Error: TAB_INSTEAD_SPC      (line:  61, col:   1):      Found tab when expecting space
expander_expand.c: OK!
expander_utils.c: OK!
lexer.c: OK!
lexer_handlers.c: OK!
lexer_multiline.c: OK!
lexer_token.c: OK!
lexer_utils.c: OK!
lexer_word.c: OK!
lexer_word_process.c: OK!
parser_ast.c: OK!
parser.c: OK!
parser_command.c: OK!
parser_command_handle.c: OK!
parser_pipeline.c: OK!
parser_print.c: Error!
Error: MIXED_SPACE_TAB      (line:  21, col:  68):      Mixed spaces and tabs
parser_print_utils.c: OK!
parser_utils.c: OK!


 % norminette ./*.h
Setting locale to en_US
./lexer_parser.h: Error!
        Error: Unrecognized line (121, 25) while parsing line [Token(type='CHAR', pos=(121, 25), value=None)]