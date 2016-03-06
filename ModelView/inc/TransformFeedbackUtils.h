#ifndef _TRANSFORMFEEDBACK
#define _TRANSFORMFEEDBACK

#include <gl.h>
#include <map>
#include <string>
#include <sstream>

using std::map;
using std::string;
using std::stringstream;

class TransformFeedbackUtils
{
private:
	map<string, unsigned> FeedbackVariables;
	unsigned VariableNumbers;
	GLuint VAO[2];
	GLuint* VBOs;
public:
	TransformFeedbackUtils() :VariableNumbers(0), VBOs(nullptr){};
	TransformFeedbackUtils(string str1)
	{
		this->VariableNumbers = 0;
		stringstream tokenizer(str1);
		while (!tokenizer.eof())
		{
			string VariableName;
			unsigned VariableComponent;
			tokenizer >> VariableName >> VariableComponent;
			FeedbackVariables[VariableName] = VariableComponent;
		}
	}
	void Attach(GLuint program)
	{
		glGenVertexArrays(2, this->VAO);
		this->VBOs = new GLuint[this->FeedbackVariables.size() * 2];
	}
};

#endif