#include "sqlWord.h"
#include "sqlParserHandle.h"
#include "sqlParserUtil.h"
#include "util/winString.h"
#include "glog/logging.h"
namespace SQL_PARSER {
	SQLSingleWord* SQLSingleWord::create(bool optional, const std::string& str)
	{
		if (str == "_A_")
			return new SQLArrayWord(optional);
		else if (str == "_AS_")
			return new SQLAnyStringWord(optional);
		else if (str == "_N_")
			return new SQLNameWord(optional);
		else if (str == "_TABLE_")
			return new SQLTableNameWord(optional);
		else if (str == "_COLUMN_")
			return new SQLColumnNameWord(optional);
		else if (str == "_B_")
			return new SQLBracketsWord(optional);
		else if (str == "_INT_")
			return new SQLIntNumberWord(optional);
		else if (str == "_FLOAT_")
			return new SQLFloatNumberWord(optional);
		else if (strncmp(str.c_str(), "_S_:", 4) == 0)
			return new SQLStringWord(optional, str.c_str() + 4);
		else if (strncmp(str.c_str(), "_C_:", 4) == 0)
			return new SQLCharWord(optional, str.c_str() + 4);
		else if (strncmp(str.c_str(), "_E_:", 4) == 0)
		{
			if (strncasecmp(str.c_str() + 4, "LOGIC", 6) == 0)
				return new SQLWordExpressions(optional, true);
			else if (strncasecmp(str.c_str() + 4, "MATH", 5) == 0)
				return new SQLWordExpressions(optional, false);
			else
				return nullptr;
		}
		else if (str == "_F_")
			return new SQLWordFunction(optional);
		else if (str == "_VL_")
			return new SQLValueListWord(optional);
		else
			return nullptr;
	}
	SQLValue* SQLCharWord::match(handle* h, const char*& sql, bool needValue)
	{
		const char* p = nextWord(sql);
		SQLStringValue* value = MATCH;
		char c = *p;
		if (c >= 'A' && c <= 'Z')
			c += 'a' - 'A';
		if (m_word != c)
		{
			LOG(ERROR) << m_comment << " do not match " << sql;
			return NOT_MATCH_PTR;
		}
		if (m_parser != nullptr || needValue)
		{
			value = new SQLStringValue(STRING_TYPE);
			value->value.assign(p, 1);
			if (needValue)
				value->ref++;
			if (m_parser != nullptr)
			{
				value->ref++;
				statusInfo* s = new statusInfo();
				s->parserFunc = m_parser;
				s->value = value;
				h->addStatus(s);
			}
		}
		LOG(ERROR) << m_comment << "match " << sql;
		sql = p + 1;
		return value;
	}
	SQLValue* SQLNameWord::match(handle* h, const char*& sql, bool needValue)
	{
		const char* p = nextWord(sql);
		std::string matchedWord;
		const char* nameStart, * nameEnd;
		uint16_t nameSize;
		SQLNameValue* value = MATCH;
		if (!getName(p, nameStart, nameSize, nameEnd))
		{
			LOG(ERROR) << m_comment << " do not match " << sql;
			return NOT_MATCH_PTR;
		}
		if (m_parser != nullptr || needValue)
		{
			value = new SQLNameValue();
			value->name.assign(nameStart, nameSize);
			if (needValue)
				value->ref++;
			if (m_parser != nullptr)
			{
				value->ref++;
				statusInfo* s = new statusInfo();
				s->parserFunc = m_parser;
				s->value = value;
				h->addStatus(s);
			}
		}
		LOG(ERROR) << m_comment << "match " << sql;
		sql = nameEnd;
		return value;
	}
	SQLValue* SQLTableNameWord::match(handle* h, const char*& sql, bool needValue)
	{
		const char* p = nextWord(sql);
		const char* nameStart[3] = { 0 };
		const char* nameEnd;
		uint16_t nameSize[3] = { 0 };
		SQLTableNameValue* value = MATCH;
		if (!getName(p, nameStart[0], nameSize[0], nameEnd))
		{
			LOG(ERROR) << m_comment << " do not match " << sql;
			return NOT_MATCH_PTR;
		}
		if (*(nameEnd + 1) == '.')
		{
			p = nameEnd + 2;
			if (!getName(p, nameStart[1], nameSize[1], nameEnd))
			{
				LOG(ERROR) << m_comment << " do not match " << sql;
				return NOT_MATCH_PTR;
			}
		}
		p = nextWord(nameEnd);
		if (strncasecmp(p, "AS", 2) == 0&& realEndOfWord(p)==p+2)
		{
			p = nextWord(p + 2);
			if(*p=='\0'|| !getName(p, nameStart[2], nameSize[2], nameEnd))
			{
				LOG(ERROR) << m_comment << " do not match for must have alias after AS in table name" << sql;
				return NOT_MATCH_PTR;
			}
		}
		else
			getName(p, nameStart[2], nameSize[2], nameEnd);
		if (m_parser != nullptr || needValue)
		{
			value = new SQLTableNameValue();
			if(nameStart[2] != nullptr)
				value->alias.assign(nameStart[2], nameSize[2]);
			if (nameStart[1] != nullptr)
			{
				value->database.assign(nameStart[0], nameSize[0]);
				value->table.assign(nameStart[1], nameSize[1]);
			}
			else
			{
				value->table.assign(nameStart[0], nameSize[0]);
			}
			if (needValue)
				value->ref++;
			if (m_parser != nullptr)
			{
				value->ref++;
				statusInfo* s = new statusInfo();
				s->parserFunc = m_parser;
				s->value = value;
				h->addStatus(s);
			}
		}
		LOG(ERROR) << m_comment << "match " << sql;
		sql = nameEnd;
		return value;
	}
	SQLValue* SQLColumnNameWord::match(handle* h, const char*& sql, bool needValue)
	{
		const char* p = nextWord(sql);
		const char* nameStart[3] = { 0 };
		const char* nameEnd;
		uint16_t nameSize[3] = { 0 };
		SQLColumnNameValue* value = MATCH;
		if (!getName(p, nameStart[0], nameSize[0], nameEnd))
		{
			LOG(ERROR) << m_comment << " do not match " << sql;
			return NOT_MATCH_PTR;
		}
		if (*(nameEnd + 1) == '.')
		{
			p = nameEnd + 2;
			if (!getName(p, nameStart[1], nameSize[1], nameEnd))
			{
				LOG(ERROR) << m_comment << " do not match " << sql;
				return NOT_MATCH_PTR;
			}
			if (*(nameEnd + 1) == '.')
			{
				p = nameEnd + 2;
				if (!getName(p, nameStart[2], nameSize[2], nameEnd))
				{
					LOG(ERROR) << m_comment << " do not match " << sql;
					return NOT_MATCH_PTR;
				}
			}
		}

		if (m_parser != nullptr || needValue)
		{
			value = new SQLColumnNameValue();
			if (nameStart[2] != nullptr)
			{
				value->database.assign(nameStart[0], nameSize[0]);
				value->table.assign(nameStart[1], nameSize[1]);
				value->columnName.assign(nameStart[2], nameSize[2]);
			}
			else if (nameStart[1] != nullptr)
			{
				value->table.assign(nameStart[0], nameSize[0]);
				value->columnName.assign(nameStart[1], nameSize[1]);
			}
			else
			{
				value->columnName.assign(nameStart[0], nameSize[0]);
			}
			if (needValue)
				value->ref++;
			if (m_parser != nullptr)
			{
				value->ref++;
				statusInfo* s = new statusInfo();
				s->parserFunc = m_parser;
				s->value = value;
				h->addStatus(s);
			}
		}
		LOG(ERROR) << m_comment << "match " << sql;
		sql = nameEnd;
		return value;
	}
	SQLValue* SQLArrayWord::match(handle* h, const char*& sql, bool needValue)
	{
		const char* p = nextWord(sql);
		SQLStringValue* value = MATCH;
		if (*p != '\'' && *p != '"')
		{
			LOG(ERROR) << m_comment << " do not match " << sql;
			return NOT_MATCH_PTR;
		}
		char quote = *p;
		const char* end = p + 1;
		while (true)
		{
			if (*end == '\0')
				break;
			if (*end == quote)
			{
				if (*(end - 1) != '\\')
				{
					if (quote == '\'' && *(end + 1) == '\'')
					{
						end += 2;
						continue;
					}
					break;
				}
				else
				{
					end++;
					continue;
				}
			}
			else
				end++;
		}
		if (*end == '\0')
		{
			LOG(ERROR) << m_comment << " do not match " << sql;
			return NOT_MATCH_PTR;
		}
		if (m_parser != nullptr || needValue)
		{
			value = new SQLStringValue(STRING_TYPE);
			value->value.assign(p + 1, end - p - 1);
			if (needValue)
				value->ref++;
			if (m_parser != nullptr)
			{
				value->ref++;
				statusInfo* s = new statusInfo();
				s->parserFunc = m_parser;
				s->value = value;
				h->addStatus(s);
			}
		}
		LOG(ERROR) << m_comment << "match " << sql;
		sql = end + 1;
		return value;
	}
	SQLValue* SQLStringWord::match(handle* h, const char*& sql, bool needValue)
	{
		const char* p = nextWord(sql);
		SQLStringValue* value = MATCH;
		if (strncasecmp(m_word.c_str(), p, m_word.size()) != 0
			|| (!isSpaceOrComment(p + m_word.size()) && p[m_word.size()] != '\0' && !isKeyChar(p[m_word.size()])))
		{
			LOG(ERROR) << m_comment << " do not match " << sql;
			return NOT_MATCH_PTR;
		}
		LOG(ERROR) << m_comment << "match " << sql;
		sql = p + m_word.size();

		if (m_parser != nullptr || needValue)
		{
			value = new SQLStringValue(STRING_TYPE);
			value->value.assign(m_word);
			if (needValue)
				value->ref++;
			if (m_parser != nullptr)
			{
				value->ref++;
				statusInfo* s = new statusInfo();
				s->parserFunc = m_parser;
				s->value = value;
				h->addStatus(s);
			}
		}
		return value;
	}
	SQLValue* SQLIntNumberWord::match(handle* h, const char*& sql, bool needValue)
	{
		const char* p = nextWord(sql);
		SQLIntNumberValue* value = MATCH;
		const char* n = p;
		bool sign = true;
		int64_t v = 0;
		if (*n == '-'||*n=='+')
		{
			if(*n=='-')
				sign = false;
			n++;
			while (*n == ' ')
				n++;
		}

		while (*n <= '9' && *n >= '0')
		{
			int64_t tmp = v * 10 + (*n) - '0';
			if (v < tmp)//overflow
			{
				LOG(ERROR) << m_comment << " do not match for int type overflow" << sql;
				return NOT_MATCH_PTR;
			}
			v = tmp;
		}
		if (!sign)
			v = -v;
		if (isKeyChar(*n))
		{
			if (n == p||*n=='.')
			{
				LOG(ERROR) << m_comment << " do not match " << sql;
				return NOT_MATCH_PTR;
			}
		}
		else if (!isSpaceOrComment(n))
		{
			LOG(ERROR) << m_comment << " do not match " << sql;
			return NOT_MATCH_PTR;
		}

		if (m_parser != nullptr || needValue)
		{
			value = new SQLIntNumberValue(v);
			if (needValue)
				value->ref++;
			if (m_parser != nullptr)
			{
				value->ref++;
				statusInfo* s = new statusInfo();
				s->parserFunc = m_parser;
				s->value = value;
				h->addStatus(s);
			}
		}
		LOG(ERROR) << m_comment << "match " << sql;
		sql = n;
		return value;
	}

	SQLValue* SQLFloatNumberWord::match(handle* h, const char*& sql, bool needValue)
	{
		const char* p = nextWord(sql);
		SQLFloatNumberValue* value = MATCH;
		const char* n = p;
		bool sign = true;
		int64_t intValue = 0, index = 0;
		double decmValue = 0;
		int8_t intValueSize = 0, decmSize = 0, indexSize = 0;
		double v = 0;
		if (*n == '-' || *n == '+')
		{
			if (*n == '-')
				sign = false;
			n++;
			while (*n == ' ')
				n++;
		}

		while (*n <= '9' && *n >= '0')
		{
			int64_t tmp = intValue * 10 + (*n) - '0';
			intValueSize++;
			if (intValue < tmp|| intValueSize>16)//overflow
			{
				LOG(ERROR) << m_comment << " do not match for int type overflow" << sql;
				return NOT_MATCH_PTR;
			}
			intValue = tmp;
		}
		if (*n == '.')
		{
			n++;
			double pos = 0.1f;
			while (*n <= '9' && *n >= '0')
			{
				decmValue += pos * ((*n) - '0');
				pos /= 10;
				decmSize++;
				if ((decmSize+ intValueSize)>16)//overflow
				{
					LOG(ERROR) << m_comment << " do not match for decmValue overflow" << sql;
					return NOT_MATCH_PTR;
				}
			}
			if (*(n - 1) == '.')
			{
				LOG(ERROR) << m_comment << " do not match for no number after [.] in float number" << sql;
				return NOT_MATCH_PTR;
			}
		}
		v = intValue + decmValue;
		if (*n == 'e' || *n == 'E')
		{
			bool indexSign = true;
			n++;
			if (*n == '-' || *n == '+')
			{
				if (*n == '-')
					indexSign = false;
				n++;
				while (*n == ' ')
					n++;
			}
			while (*n <= '9' && *n >= '0')
			{
				int64_t tmp = indexSize * 10 + (*n) - '0';
				indexSize++;
				if (indexSize < tmp||indexSize>4932)//overflow
				{
					LOG(ERROR) << m_comment << " do not match for index overflow" << sql;
					return NOT_MATCH_PTR;
				}
				if (indexSign)
					v *= 10;
				else
					v /= 10;
				indexSize = tmp;
			}
			if (!indexSign)
				index = -index;
		}

		if (isKeyChar(*n))
		{
			if (n == p)
			{
				LOG(ERROR) << m_comment << " do not match " << sql;
				return NOT_MATCH_PTR;
			}
		}
		else if (!isSpaceOrComment(n))
		{
			LOG(ERROR) << m_comment << " do not match " << sql;
			return NOT_MATCH_PTR;
		}

		if (m_parser != nullptr || needValue)
		{
			if (!sign)
				v = -v;
			value = new SQLFloatNumberValue(v);
			if (needValue)
				value->ref++;
			if (m_parser != nullptr)
			{
				value->ref++;
				statusInfo* s = new statusInfo();
				s->parserFunc = m_parser;
				s->value = value;
				h->addStatus(s);
			}
	}
		LOG(ERROR) << m_comment << "match " << sql;
		sql = n;
		return value;
	}
	SQLValue* SQLAnyStringWord::match(handle* h, const char*& sql, bool needValue)
	{
		const char* p = nextWord(sql);
		const char* end = endOfWord(p);
		SQLStringValue* value = MATCH;
		if (end == nullptr)
		{
			LOG(ERROR) << m_comment << " do not match " << sql;
			return NOT_MATCH_PTR;
		}
		if (isKeyWord(p, end - p))
		{
			LOG(ERROR) << m_comment << " do not match " << sql;
			return NOT_MATCH_PTR;
		}
		if (m_parser != nullptr || needValue)
		{
			value = new SQLStringValue(STRING_TYPE);
			value->value.assign(p, end - p);
			if (needValue)
				value->ref++;
			if (m_parser != nullptr)
			{
				value->ref++;
				statusInfo* s = new statusInfo();
				s->parserFunc = m_parser;
				s->value = value;
				h->addStatus(s);
			}
		}
		LOG(ERROR) << m_comment << "match " << sql;
		sql = end;
		return value;
	}
	SQLValue* SQLOperatorWord::match(handle* h, const char*& sql, bool needValue)
	{
		OPERATOR op = parseOperation(sql);
		if (op == NOT_OPERATION)
			return NOT_MATCH_PTR;
		return new SQLOperatorValue(op);
	}
	SQLValue* SQLValueListWord::match(handle* h, const char*& sql, bool needValue)
	{
		const char* pos = nextWord(sql);
		if (*pos != '(')
			return NOT_MATCH_PTR;
		pos = nextWord(pos + 1);
		if (*pos == ')')//not allowed empty list
			return NOT_MATCH_PTR;
		SQLValueList* vlist = needValue ? new SQLValueList() : nullptr;
		SQLValue* v;
		SQLValueType valuesType = SQLValueType::MAX_TYPE;
		while (*pos != '\0')
		{
			if (NOT_MATCH_PTR != (v = expressionWord.match(nullptr, pos, needValue)))
			{
				if (needValue)
					vlist->values.push_back(v);
			}
			else if (NOT_MATCH_PTR != (v = funcWord.match(nullptr, pos, needValue)) ||
				NOT_MATCH_PTR != (v = strWord.match(nullptr, pos, needValue)) ||
				NOT_MATCH_PTR != (v = intWord.match(nullptr, pos, needValue)) ||
				NOT_MATCH_PTR != (v = floatWord.match(nullptr, pos, needValue)))
			{
				if (needValue)
					vlist->values.push_back(v);
				if (valuesType == SQLValueType::MAX_TYPE)
					valuesType = v->type;
				else if (valuesType != v->type)
				{
					if (vlist != nullptr)
						delete vlist;
					return NOT_MATCH_PTR;
				}
			}
			else
			{
				if (vlist != nullptr)
					delete vlist;
				return NOT_MATCH_PTR;
			}
			pos = nextWord(pos);
			if(*pos==',')
				pos = nextWord(pos + 1);
			else if (*pos == ')')
				break;
			else
			{
				if (vlist != nullptr)
					delete vlist;
				return NOT_MATCH_PTR;
			}
		}
		sql = pos+1;
		return vlist;
	}
	SQLValue* SQLBracketsWord::match(handle* h, const char*& sql, bool needValue)
	{
		const char* p = nextWord(sql);
		SQLStringValue* value = MATCH;

		if (*p != '(')
		{
			LOG(ERROR) << m_comment << " do not match " << sql;
			return NOT_MATCH_PTR;
		}
		const char* end = p + 1;
		int32_t bracketCount = 1;
		while (*end != '\0')
		{
			if (*end == '(')
				bracketCount++;
			else if (*end == ')')
			{
				if (--bracketCount <= 0)
					break;
			}
			end++;
		}
		if (*end == '\0')
		{
			LOG(ERROR) << m_comment << " do not match " << sql;
			return NOT_MATCH_PTR;
		}
		if (m_parser != nullptr || needValue)
		{
			value = new SQLStringValue(STRING_TYPE);
			value->value.assign(p, end - p);
			if (needValue)
				value->ref++;
			if (m_parser != nullptr)
			{
				value->ref++;
				statusInfo* s = new statusInfo();
				s->parserFunc = m_parser;
				s->value = value;
				h->addStatus(s);
			}
		}
		LOG(ERROR) << m_comment << "match " << sql;
		sql = end + 1;
		return value;
	}
	SQLValue* SQLWordArray::match(handle* h, const char*& sql, bool needValue)
	{
		SQLValue* rtv = nullptr;
		bool matched = false;
		const char* savePoint = sql, * beforeLoop = nullptr;
		statusInfo* top = h->end;
		do
		{
			for (std::list<SQLWord*>::iterator iter = m_words.begin();
				iter != m_words.end(); iter++)
			{
				SQLWord* s = *iter;
				const char* str = nextWord(sql);
				if (s == nullptr)
					continue;
				if ((rtv = s->match(h, str)) == NOT_MATCH_PTR)
				{
					if (m_or)
						continue;
					if (s->m_optional)
					{
						rtv = MATCH;
						continue;
					}
					else
						break;
				}
				else
				{
					sql = str;
					if (m_or)
						break;
				}
			}
			if (rtv == NOT_MATCH_PTR)
			{
				if (m_loop && matched)
				{
					if (beforeLoop != nullptr)
						sql = beforeLoop;
					rtv = MATCH;
				}
				break;
			}
			else
				matched = true;
			if (m_loop)
			{
				if (m_loopCondition != nullptr)
				{
					beforeLoop = sql;
					const char* str = nextWord(sql);
					if (m_loopCondition->match(nullptr, str) != NOT_MATCH_PTR)
					{
						sql = str;
						continue;
					}
					else
						break;
				}
			}
			else
				break;
		} while (1);

		if (rtv == NOT_MATCH_PTR) //rollback
		{
			for (statusInfo* s = top ? top->next : nullptr; s != nullptr;)
			{
				statusInfo* tmp = s->next;
				delete s;
				s = tmp;
			}
			h->end = top;
			if (h->end != nullptr)
				h->end->next = nullptr;
			else
				h->head = nullptr;
			sql = savePoint;
			LOG(ERROR) << m_comment << " do not match " << sql;
		}
		else
		{
			LOG(ERROR) << m_comment << "  match " << savePoint;
			if (m_sqlType != UNSUPPORT)
				h->type = m_sqlType;
		}
		return rtv;
	}
	SQLWordExpressions::SQLWordExpressions(bool optional,bool logicOrMath) :SQLSingleWord(optional, S_EXPRESSION), logicOrMath(logicOrMath), opWord(false), intWord(false),floatWord(false), strWord(false), nameWord(false), func(nullptr)
	{
		func = new SQLWordFunction(false);
		valueList = new SQLValueListWord(false);
	}
	SQLWordExpressions::~SQLWordExpressions()
	{
		delete func;
		delete valueList;
	}
	SQLValue* SQLWordExpressions::matchValue(const char*& sql, bool needValue, std::stack<SQLOperatorValue*>& opStack, std::list<SQLValue*>& valueList)
	{
		SQLValue* value;
		if ((value = this->valueList->match(nullptr, sql, needValue)) != NOT_MATCH_PTR)
		{
			if (opStack.empty() || (opStack.top()->opera != IN_ && opStack.top()->opera != NOT_IN))
			{
				if (value != nullptr)
					delete value;
				return NOT_MATCH_PTR;
			}
			else
			{
				valueList.push_back(value);
				return value;
			}
		}
		else if ((value = func->match(nullptr, sql, needValue)) != NOT_MATCH_PTR ||
			(value = intWord.match(nullptr, sql, needValue)) != NOT_MATCH_PTR ||
			(value = floatWord.match(nullptr, sql, needValue)) != NOT_MATCH_PTR ||
			(value = nameWord.match(nullptr, sql, needValue)) != NOT_MATCH_PTR ||
			(value = strWord.match(nullptr, sql, needValue)) != NOT_MATCH_PTR)
		{
			valueList.push_back(value);
			return value;
		}
		else
		{
			SQLValue* opValue;
			if ((opValue = opWord.match(nullptr, sql, true)) != NOT_MATCH_PTR &&
				!operationInfos[static_cast<SQLOperatorValue*>(opValue)->opera].hasLeftValues&&
				operationInfos[static_cast<SQLOperatorValue*>(opValue)->opera].hasRightValue)
			{
				if ((value = func->match(nullptr, sql, needValue)) != NOT_MATCH_PTR ||
					(value = intWord.match(nullptr, sql, needValue)) != NOT_MATCH_PTR ||
					(value = floatWord.match(nullptr, sql, needValue)) != NOT_MATCH_PTR ||
					(value = nameWord.match(nullptr, sql, needValue)) != NOT_MATCH_PTR ||
					(value = strWord.match(nullptr, sql, needValue)) != NOT_MATCH_PTR)
				{
					while (!opStack.empty() && operationInfos[opStack.top()->opera].priority < operationInfos[static_cast<SQLOperatorValue*>(opValue)->opera].priority)
					{
						valueList.push_back(opStack.top());
						opStack.pop();
					}
					opStack.push(static_cast<SQLOperatorValue*>(opValue));
					valueList.push_back(value);
					return value;
				}
			}
		}
		return NOT_MATCH_PTR;
	}
	SQLValue* SQLWordExpressions::matchOperation(const char*& sql, bool needValue, std::stack<SQLOperatorValue*>& opStack, std::list<SQLValue*>& valueList)
	{
		SQLValue* opValue;
		if ((opValue = opWord.match(nullptr, sql, true)) != NOT_MATCH_PTR)
		{
			if (operationInfos[static_cast<SQLOperatorValue*>(opValue)->opera].hasLeftValues&&
				operationInfos[static_cast<SQLOperatorValue*>(opValue)->opera].hasRightValue)
			{
				while (!opStack.empty() && operationInfos[opStack.top()->opera].priority < operationInfos[static_cast<SQLOperatorValue*>(opValue)->opera].priority)
				{
					valueList.push_back(opStack.top());
					opStack.pop();
				}
				opStack.push(static_cast<SQLOperatorValue*>(opValue));
				return opValue;
			}
			else
			{
				delete opValue;
				return NOT_MATCH_PTR;
			}
		}
		else
			return NOT_MATCH_PTR;
	}
	SQLValue* SQLWordExpressions::matchLBrac(const char*& sql, bool needValue, std::stack<SQLOperatorValue*>& opStack, std::list<SQLValue*>& valueList)
	{
		const char* ptr = nextWord(sql);
		if (*ptr == '(')
		{
			SQLOperatorValue* op = new SQLOperatorValue(LEFT_BRACKET);
			opStack.push(op);
			sql = ptr + 1;
			return op;
		}
		else
			return NOT_MATCH_PTR;
	}
	SQLValue* SQLWordExpressions::matchRBrac(const char*& sql, bool needValue, std::stack<SQLOperatorValue*>& opStack, std::list<SQLValue*>& valueList)
	{
		const char* ptr = nextWord(sql);
		if (*ptr == ')')
		{
			SQLOperatorValue* op = nullptr;
			while (!opStack.empty() && (op = opStack.top(), op->opera != LEFT_BRACKET))
			{
				opStack.pop();
				valueList.push_back(op);
			}
			if (op == nullptr || op->opera != LEFT_BRACKET)
				return NOT_MATCH_PTR;
			sql = ptr + 1;
			return MATCH;
		}
		return NOT_MATCH_PTR;
	}	/*trans mid-prefix to back-prefix*/
	SQLValue* SQLWordExpressions::match(handle* h, const char*& sql, bool needValue)
	{
		std::stack<SQLOperatorValue*> opStack;
		std::list<SQLValue*> valueList;
		SQLExpressionValue* value = nullptr;
		const char* pos = nextWord(sql);
		bool status = true;
		do {
			if (status)
			{
				if (matchValue(pos, needValue, opStack, valueList) != NOT_MATCH_PTR)
					status = false;
				else
					goto NOT_MATCH;
			}
			else
			{
				if (matchOperation(pos, needValue, opStack, valueList) != NOT_MATCH_PTR)
				{
					pos = nextWord(pos);
					if (matchValue(pos, needValue, opStack, valueList) != NOT_MATCH_PTR)
					{
						pos = nextWord(pos);
						matchRBrac(pos, needValue, opStack, valueList);
						break;
					}
					else if (matchLBrac(pos, needValue, opStack, valueList) != NOT_MATCH_PTR)
					{
						status = true;
						break;
					}
					else
						goto NOT_MATCH;
				}
				else
				{
					if (valueList.size() + opStack.size() <= 1)
						goto NOT_MATCH;
					else
						goto CHECK;
				}
			}
			pos = nextWord(pos);
		} while (*pos != '\0');

	CHECK:
		while (!opStack.empty())
		{
			if (opStack.top()->opera == LEFT_BRACKET)
				goto NOT_MATCH;
		}
		value = new SQLExpressionValue();
		value->valueStack = new SQLValue * [valueList.size() + opStack.size()];
		value->count = 0;
		for (std::list <SQLValue*>::iterator iter = valueList.begin(); iter != valueList.end(); iter++)
			value->valueStack[value->count++] = *iter;
		while (!opStack.empty())
		{
			value->valueStack[value->count++] = opStack.top();
			opStack.pop();
		}
		if(value->count == 0||value->valueStack[value->count-1]->type!= OPERATOR_TYPE||
			((operationInfos[static_cast<SQLOperatorValue*>(value->valueStack[value->count - 1])->opera].optType==LOGIC)^logicOrMath)
			|| !value->check())
		{
			delete value;
			return NOT_MATCH_PTR;
		}
		sql = pos;
		return value;
	NOT_MATCH:
		for (std::list <SQLValue*>::iterator iter = valueList.begin(); iter != valueList.end(); iter++)
		{
			if (*iter != nullptr)
				delete* iter;
		}
		while (!opStack.empty())
		{
			delete opStack.top();
			opStack.pop();
		}
		return NOT_MATCH_PTR;
	}

	SQLValue* SQLWordFunction::match(handle* h, const char*& sql, bool needValue)
	{
		SQLValue* name = nullptr;
		const char* pos = nextWord(sql);
		if ((name = asWord.match(nullptr, pos, needValue ||  m_parser != nullptr)) == NOT_MATCH_PTR)
			return NOT_MATCH_PTR;
		pos = nextWord(pos);
		if (*pos != '(')
			return NOT_MATCH_PTR;
		SQLFunctionValue* sfv = MATCH;

		if (needValue || m_parser != nullptr)
		{
			if (name == nullptr)
				return NOT_MATCH_PTR;
			sfv = new SQLFunctionValue();
			sfv->funcName = static_cast<SQLStringValue*>(name)->value;
			delete name;
		}

		pos = nextWord(pos + 1);
		while (*pos != '\0')
		{
			if (*pos == ')')
			{
				sql = pos + 1;
				if (needValue)
					sfv->ref++;
				if (h != nullptr && m_parser != nullptr)
				{
					statusInfo* s = new statusInfo();
					sfv->ref++;
					s->parserFunc = m_parser;
					s->value = sfv;
					h->addStatus(s);
				}
				return sfv;
			}
			SQLValue* value = MATCH;
			if ((value = expressionWord.match(h, pos, needValue)) != NOT_MATCH_PTR ||
				(value = intWord.match(h, pos, needValue)) != NOT_MATCH_PTR ||
				(value = floatWord.match(h, pos, needValue)) != NOT_MATCH_PTR ||
				(value = nameWord.match(h, pos, needValue)) != NOT_MATCH_PTR ||
				(value = strWord.match(h, pos, needValue)) != NOT_MATCH_PTR ||
				(value = match(nullptr, pos, needValue)) != NOT_MATCH_PTR)
			{
				if (sfv != nullptr)
					sfv->argvs.push_back(value);
			}
			else
			{
				if (sfv != nullptr)
					delete sfv;
				return NOT_MATCH_PTR;
			}

			pos = nextWord(pos);
			if (*pos == ',')
			{
				pos = nextWord(pos + 1);
				if (*pos == ')')
				{
					if (sfv != nullptr)
						delete sfv;
					return NOT_MATCH_PTR;
				}
			}
			else
			{
				if (*pos != ')')
				{
					if (sfv != nullptr)
						delete sfv;
					return NOT_MATCH_PTR;
				}
			}
		}
		if (sfv != nullptr)
			delete sfv;
		return NOT_MATCH_PTR;
	}
}
