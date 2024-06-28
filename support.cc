#include "common-headers.hh"

int temp_count=0;
map <string, int> memory; // Stores values of variables during interpretation

//////////////////////// PROCESSING REDUCTIONS /////////////////////////////////

Ast * process_NUM(string * name)
{
	int num;

	try
	{
	  num = stoi(*name);
	} 
	catch (const std::out_of_range& ex)
	{
	  std::cerr << "Number too large: " << name << ex.what() << std::endl;
	  exit(1);
	}

	Ast * ast = new Number_Expr_Ast<int>(num);
	assert (ast != NULL);
	return ast;	
}

Ast * process_ID(string * name)
{
	Type_Info *type_info = symtab.find(*name)->second;
	Ast * ast = new Name_Expr_Ast(*name, type_info);
	assert (ast != NULL);
	return ast;	
}

Ast * process_Expr(Ast *left, op_type op, Ast *right)
{
	int result;
	Ast * ast;
	Type_Info *type_info;

	switch (op)
	{
		case PLUS:	type_info = type_check_binary(left, right, "plus");
				ast = new Plus_Expr_Ast(left, right, type_info); assert(ast); break;
		case MINUS:	type_info = type_check_binary(left, right, "minus");
				ast = new Minus_Expr_Ast(left, right, type_info); assert(ast); break;
		case MULT:	type_info = type_check_mult(left, right); 
				ast = new Mult_Expr_Ast(left, right, type_info); assert(ast); break;
		case DIV:	type_info = type_check_binary(left, right, "div");
				ast = new Div_Expr_Ast(left, right, type_info); assert(ast); break;
                case MATMUL:
			type_info = type_check_matmul(left, right);
			ast = new MatMul_Expr_Ast(left, right, type_info);
			assert (ast);
			break;
		case UMINUS:
			if (right != NULL)
			{
				exit(1);
			}
			else	ast = new UMinus_Expr_Ast(left);
			break;
		default:
			cerr << "Wrong operator type" << endl;
			exit(1);
			break;
	}
	return ast; 
}

Ast * process_Asgn(string *lhs_name, Ast *rhs)
{
        assert (lhs_name);
	string name = *lhs_name;
	type_check_assign (name, rhs);
	Ast * id = new Name_Expr_Ast(name, symtab.find(name)->second);
	assert (id);
	Ast *ast = new Assignment_Stmt_Ast(id, rhs);
	assert (ast);
	return ast;	
}

list<Ast *> * process_Stmt_List(list<Ast *> *ast_list, Ast *ast)
{
	assert (ast_list != NULL);
	ast_list->push_back(ast);
	return ast_list;	
}

list<Ast *> * init_Stmt_List(Ast *ast)
{
	list <Ast*> *ast_list = new list <Ast*>;
	assert (ast_list != NULL);
	ast_list->push_back(ast);
	return ast_list;	
}

//Defining functions of Type_Info class
Type_Info::Type_Info(var_type bt)
{
	this->base_type = bt;
}
Type_Info::Type_Info(var_type bt, int nd, int fs, int ss)
{
	this->base_type = bt;
	this->dim_count = nd;
	this->first_dim_size = fs;
	this->second_dim_size = ss;
}
string Type_Info::base_type_name()
{
	if(this->base_type == INT32)
	{
		return "INT32";
	}
	else if (this->base_type == INT8)
	{
		return "INT8";
	}
        return "Invalid base type name from Type_Info";
}
var_type Type_Info::get_base_type()
{
	return this->base_type;
}
int Type_Info::get_number_of_dimensions()
{
	return this->dim_count;
}
int Type_Info::get_size_of_first_dim()
{
	return this->first_dim_size;
}
int Type_Info::get_size_of_second_dim()
{
	return this->second_dim_size;
}
bool Type_Info::is_tensor_type()
{
	return this->dim_count > 0;
}

bool found_in_symbol_table(string * name)
{
    return (symtab.find(*name) != symtab.end());
}

void add_symbol_table_entry(string * name, Type_Info * t)
{
    if (found_in_symbol_table(name)) {
        cout<< "\nError: Found " << *name << " in Symbol Table: Discard\n";
        return;
    }
    cout<< "\nEntering new data " << *name << " to Symbol Table";
    symtab.insert({*name, t});
}

void show_symbol_table()
{
    cout << "\nSymbol Table View\n";
    for (auto x : symtab) {
        cout << x.first << " " << (x.second)->base_type_name() << endl;
    }
}

int get_int_from_string (string * name)
{
    int num = stoi(*name);
    return num;
}
