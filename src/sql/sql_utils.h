/**
 * @author  yaocoder <yaocoder@gamil.com>
 * @section DESCRIPTION
 * 组装SQL查询语句
 */

#ifndef SQL_UTILS_H_
#define SQL_UTILS_H_

#include <string>
#include <map>
using namespace std;
namespace SQL_UTILS
{

enum ConditionType
{
	CT_EQUAL = 1, CT_EQUAL_STRING, CT_NOT_EQUAL, CT_ABOVE, CT_BELOW, CT_FUZZY, CT_AREA, CT_GROUP,
};


// 查询条件结构
class SelCondition
{
public:
	//针对范围
	void SetCondition(string valueFirst, string valueSecond, int conType)
	{
		m_strValueFirst = valueFirst;
		m_strValueSecond = valueSecond;
		m_conType = conType;
	}
	//针对一个条件
	void SetCondition(string valueFirst, int conType)
	{
		m_strValueFirst = valueFirst;
		m_conType = conType;
	}

	string m_strValueFirst;
	string m_strValueSecond;
	int	   m_conType;

};

typedef map<string, SelCondition> MAP_SELECTCONDITION;

// 返回不带 where 的条件语句，并且是以 and 开头，例如 and id>8 and name=aiht
string AssembleCondition(const map<string, SelCondition>& mapConditions)
{
	string strCondition = "";

	typedef map<string, SelCondition>::const_iterator CI;
	for (CI p = mapConditions.begin(); p != mapConditions.end(); ++p)
	{
		string strCmd;
		string colName = p->first;

		//进行查询类型的判定拼装相应条件
		switch (p->second.m_conType)
		{
		case CT_ABOVE:
			strCmd = string(" AND ") + colName + string(" > '") + p->second.m_strValueFirst + string("'");
			break;
		case CT_BELOW:
			strCmd = string(" AND ") + colName + string(" < '") + p->second.m_strValueFirst + string("'");
			break;
		case CT_EQUAL_STRING:
			strCmd = string(" AND ") + colName + string(" = '") + p->second.m_strValueFirst + string("'");
			break;
		case CT_EQUAL:
			strCmd = string(" AND ") + colName + string(" = ") + p->second.m_strValueFirst;
			break;
		case CT_NOT_EQUAL:
			strCmd = string(" AND ") + colName + string(" != '") + p->second.m_strValueFirst + string("'");
			break;
		case CT_FUZZY:
			strCmd = string(" AND ") + colName + string(" like '%") + p->second.m_strValueFirst + string("%'");
			break;
		case CT_AREA:
			strCmd = string(" AND ") + colName + string(" BETWEEN '") + p->second.m_strValueFirst + string("' AND '")
					+ p->second.m_strValueSecond + string("'");
			break;
		case CT_GROUP:
			strCmd = string(" group by StartTime ");
			break;
		default:
			;

		}
		strCondition += strCmd;
	}
	return strCondition;
}

}

#endif
