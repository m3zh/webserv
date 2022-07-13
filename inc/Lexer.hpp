# include "Parser.hpp"

class AToken;

class Lexer
{
	private:

		Lexer();
		Lexer(Lexer const &rhs);
		Lexer& operator=(Lexer const &rhs);

	protected:

		std::string 				keyType;
		std::string 				valueType;
		std::string 				sepType;
		std::string 				words[];
		std::map<AToken&, AToken&>	tokens;

	public:

		Lexer(std::string fileName); // type to replace by FILE* ?
		~Lexer();

		std::string					tokenize(std::string fileName);
		std::map<AToken&, AToken&> 	createTokenMap(); // find cooler name
		

};