//
// FILE: gsminstr.h -- definition of Instruction classes for GSM's
//                     instruction queue subsystem
//                     companion to GSM
//
// $Id$
//



#ifndef GSMINSTR_H
#define GSMINSTR_H


#include "gtext.h"
#include "gmisc.h"
#include "portion.h"

class gclExpression     {
  public:
    virtual ~gclExpression()  { }
    
    virtual PortionSpec Type(void) const   { return porANYTYPE; }
    virtual Portion *Evaluate(void) = 0;
};


class gclQuitOccurred : public gException {
 public:
  ~gclQuitOccurred() { }
  gText Description(void) const { return "Quit expression executed"; }
};


class gclQuitExpression : public gclExpression  {
  public:
    gclQuitExpression(void)  { }
    virtual ~gclQuitExpression()  { } 

    PortionSpec Type(void) const;
    Portion *Evaluate(void);
};


class gclSemiExpr : public gclExpression  {
  private:
    gclExpression *lhs, *rhs;

  public:
    gclSemiExpr(gclExpression *l, gclExpression *r);
    virtual ~gclSemiExpr();

    PortionSpec Type(void) const;
    Portion *Evaluate(void);
};

class gclReqParameterList  {
  private:
    gList<gclExpression *> exprs;

  public: 
    gclReqParameterList(void);
    gclReqParameterList(gclExpression *);
    ~gclReqParameterList();
  
    void Append(gclExpression *);

    int NumParams(void) const;
    gclExpression *operator[](int index) const;
};


class gclOptParameterList  {
  private:
    gList<gText> names;
    gList<gclExpression *> exprs;

  public: 
    gclOptParameterList(void);
    gclOptParameterList(const gText &, gclExpression *);
    virtual ~gclOptParameterList();
  
    void Append(const gText &, gclExpression *);

    int NumParams(void) const;
    gclExpression *operator[](int index) const;
    gText FormalName(int index) const;
};

class gclParameterList   {
  friend class gclFunctionCall;
  private:
    gclReqParameterList *req;
    gclOptParameterList *opt;

  public:
    gclParameterList(void);
    gclParameterList(gclReqParameterList *r);
    gclParameterList(gclOptParameterList *o);
    gclParameterList(gclReqParameterList *r, gclOptParameterList *o);
    ~gclParameterList();
};


class gclFunctionCall : public gclExpression   {
  private:
    gText name;
    gclParameterList *params;
    
    Portion *(*funcptr)(Portion **);
    PortionSpec type;    

    void AttemptMatch(void);

  public:
    gclFunctionCall(const gText &name);
    gclFunctionCall(const gText &name, gclExpression *op);
    gclFunctionCall(const gText &name,
                    gclExpression *op1, gclExpression *op2);
    gclFunctionCall(const gText &name, gclParameterList *params);
    virtual ~gclFunctionCall();

    PortionSpec Type(void) const;
    Portion *Evaluate(void);
};


class gclAssignment : public gclExpression  {
  private:
    gclExpression *variable, *value;

  public:
    gclAssignment(gclExpression *value, gclExpression *var);
    virtual ~gclAssignment();

    Portion *Evaluate(void);
};

class gclUnAssignment : public gclExpression  {
  private:
    gclExpression *variable;

  public:
    gclUnAssignment(gclExpression *var);
    virtual ~gclUnAssignment();

    Portion *Evaluate(void);
};

class FuncDescObj;

class gclFunctionDef : public gclExpression  {
  private:
    FuncDescObj *func;
    gclExpression *body;

  public:
    gclFunctionDef(FuncDescObj *f, gclExpression *b);
    virtual ~gclFunctionDef();

    Portion *Evaluate(void);
};

class gclConstExpr : public gclExpression    {
  private:
    Portion *value;

  public:
    gclConstExpr(Portion *value);
    virtual ~gclConstExpr();

    PortionSpec Type(void) const;
    Portion *Evaluate(void);
};

class gclListConstant : public gclExpression  {
  private:
    gList<gclExpression *> values;

  public:
    gclListConstant(void);
    gclListConstant(gclExpression *);
    virtual ~gclListConstant();

    void Append(gclExpression *);

    Portion *Evaluate(void);
};


class gclVarName : public gclExpression   {
  private:
    Portion *value;

  public:
    gclVarName(const gText &);
    virtual ~gclVarName();

    Portion *Evaluate(void);
};


class gclConditional : public gclExpression  {
  private:
    gclExpression *guard, *truebr, *falsebr;

  public:
    gclConditional(gclExpression *guard, gclExpression *iftrue);
    gclConditional(gclExpression *guard,
                   gclExpression *iftrue, gclExpression *iffalse);
    virtual ~gclConditional();

    Portion *Evaluate(void);
};

class gclWhileExpr : public gclExpression  {
  private:
    gclExpression *guard, *body;

  public:
    gclWhileExpr(gclExpression *guard, gclExpression *body);
    virtual ~gclWhileExpr();

    Portion *Evaluate(void);
};

class gclForExpr : public gclExpression  {
  private:
    gclExpression *init, *guard, *step, *body;

  public:
    gclForExpr(gclExpression *init, gclExpression *guard,
               gclExpression *step, gclExpression *body);
    virtual ~gclForExpr();

    Portion *Evaluate(void);
};


#endif // GSMINSTR_H


