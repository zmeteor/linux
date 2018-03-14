/*************************************************************************
	> File Name: checkpasswd.c
	> Author: 
	> Mail: 
	> Created Time: 2018年02月26日 星期一 12时25分04秒
 ************************************************************************/

//检测密码强度

#include "config.h"

//密码等级
PasswordLevel GetPasswordLevel(int score)
{
    if(score < 20) 
    {
        return VERYWEEK;
    }
    else if(score < 40) 
    {
        return WEEK;
    }
    else if(score < 60) 
    {
        return AVERAGE;
    }
    else if(score < 80)
    {
        return  GOOD;
    }
    else if(score < 90) 
    {
        return STRONG;
    }
    else
    {
       return VERYSTRONG;
    }
}

//密码得分
int GetPasswordScore(const char *password)
{
    RuleParameter rp= {0};
    beginProcess(password,&rp);
    return countScore(&rp);
}
//调用其他计算函数
void beginProcess(const char *password,RuleParameter *rp)
{
    if(password==NULL || rp==NULL)
    {
        return; 
    }

    int i,j;

    char *pass_nospace = (char *)malloc(strlen(password)+1);  //存储原始密码
    char *pass_nospace_lower = (char *)malloc(strlen(password)+1);  //存储转为小写的密码

    if(!pass_nospace || !pass_nospace_lower)
    {
        exit(EXIT_FAILURE);
    }

    for(i = 0 ,j = 0 ; password[i] != '\0'; i++)
    {
        if(!isspace(password[i])) 
        {
            pass_nospace[j] = password[i];
            pass_nospace_lower[j] = tolower(password[i]);  //转小写
            ++j;
        }
    }
    pass_nospace[j] = '\0';

    rp->nLength = strlen(pass_nospace);

    countRepeat(pass_nospace_lower,rp);
    countNumbers(pass_nospace,rp);
    countSeqNumbers(pass_nospace,rp);

    free(pass_nospace);
    free(pass_nospace_lower);
}

//填充重复单元字段
void countRepeat(const char *password, RuleParameter *rp)
{
    if(!password || !rp)
    {
        return;
    }


    int i;
    for(i=0; i<rp->nLength; i++)
    {
        ++rp->RepeatChar[password[i]];
    }
}

//填充大写、小写、数字、符号及他们的连续值字段
void countNumbers(const char *password, RuleParameter *rp)
{
    if(!password || !rp)
    {
        return;
    }

    int i;
    int last_upper_pos = -1;  //最后一个大写字母的位置
    int last_lower_pos = -1;  //最后一个小写字母的位置
    int last_digit_pos = -1;  //最后一个数子的位置

    for(i = 0; i < rp->nLength; i++)
    {
        if(isupper(password[i])) //计算大写字母个数其连续个数
        {
            if(last_upper_pos != -1 && last_upper_pos + 1 == i)
            {
                ++rp->nConsecUpper;
            }
            last_upper_pos = i;
            ++rp->nUpper;
        } 
        else if(islower(password[i])) //计算小写字母个数及其连续个数 
        {
            if(last_lower_pos != -1 && last_lower_pos + 1 == i) 
            {
                ++rp->nConsecLower;
            }
            last_lower_pos = i;
            ++rp->nLower;
        } 
        else if(isdigit(password[i]))//计算数字个数及其连续个数 
        {
            if(last_digit_pos != -1 && last_digit_pos + 1 == i) 
            {
                ++rp->nConsecDigit;
            }
            if(i > 0 && i < rp->nLength-1) 
            {
                ++rp->nMidDigitSymbol; //中间部分的数字
            }
            last_digit_pos = i;
            ++rp->nDigit;
        } 
        else  //计算特殊字符个数
        {
            if(i > 0 && i < rp->nLength-1) 
            {
                ++rp->nMidDigitSymbol;  //中间部分的字符
            }
            ++rp->nSymbol;
        }
    }

   /*字母、数字、字符最低需求次数*/
    if(rp->nLower>0)
    {
        ++rp->nRequirement;
    }
    if(rp->nUpper>0)
    {
        ++rp->nRequirement;
    }
    if(rp->nDigit>0)
    {
        ++rp->nRequirement;
    }
    if(rp->nSymbol>0)
    {
        ++rp->nRequirement;
    }
}

//填充连续递增/递减字符的个数子段
void countSeqNumbers(const char *password,RuleParameter *rp)
{
    if(!password || !rp || rp->nLength<3)
    {
        return;
    }

    int inc_count = 1;  //连续递增字符个数
    int dec_count = 1;  //连续递减字符个数
    int i = 0;

    for(i = 1; i<rp->nLength; i++)
    {
        if(isalnum(password[i]) && isalnum(password[i-1])) //若连续字符均为数字或字母 
        {
            if(password[i]-password[i-1] == 1) //递增
            {
                if(dec_count < 3)
                {
                    ++inc_count;
                }
                dec_count = 1;
            }
            else if(password[i]-password[i-1] == -1) //递减
            {
                if(inc_count < 3)
                {
                    ++dec_count;
                }
                inc_count = 1;
            } 
            else 
            {
                inc_count = dec_count = 1;
            }
        }
        else
        {
            inc_count = dec_count = 1;
        }

        if(inc_count >= 3 || dec_count >= 3) 
        {
            ++rp->nSequence; 
        }
    }
}

//计算得分
int countScore(const RuleParameter *rp)
{
    if(!rp || rp->nLength==0)
    {
        return 0;
    }

    int score=0;
    int i;
    int n;

    score += rp->nLength * 4;  //密码长度*4
    if(rp->nUpper != 0)
    {
        score += (rp->nLength - rp->nUpper) * 2; //（长度-大写字母数）* 2
    }
    if(rp->nLower != 0)
    {
        score += (rp->nLength - rp->nLower) * 2; //（长度-小写字母数）* 2
    }
    if(rp->nLength != rp->nDigit)
    {
        score += rp->nDigit * 4; //数字个数 * 4
    }
    score += rp->nSymbol * 6;   //特殊字符个数 * 6
    score += rp->nMidDigitSymbol * 2;  //中间部分的数字或字符个数 * 2

    //长度>=8,大写、小写英文字母、数字、特殊符号 >= 3种
    if(rp->nLength >= 8 && rp->nRequirement >= 3) 
    {
        score += (rp->nRequirement+1) * 2;
    }

    //全数字或全字母减分
    if((rp->nDigit == rp->nLength) || ((rp->nLower + rp->nUpper) == rp->nLength))
    {
        score -= rp->nLength;
    }

    //重复字符数减分
    for(i=0; i<CHARNUM; ++i) 
    {
        n = rp->RepeatChar[i];
        if(n>1)
        {
            score -= n*(n-1);
        }
    }

    //连续数字、字母减分
    score -= rp->nConsecDigit * 2;
    score -= rp->nConsecLower * 2;
    score -= rp->nConsecUpper * 2;
    score -= rp->nSequence * 3;

    if(score<0)
    {
        score = 0;
    }
    
    if(score>100) 
    {
        score = 100;
    }

   return score;
}


