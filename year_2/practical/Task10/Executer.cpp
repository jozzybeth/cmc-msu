#include "Executer.h"

template <class T, class T_EL>
void from_poliz ( T & st, T_EL & i ) {
    i = st.top(); st.pop();
}

extern std::vector <Ident> TID;
void Executer::execute(vector<Lex>& poliz) 
{
        Lex pc_el, o1, o2;
        stack <Lex> args; //стек для аргументов операций
        stack <Lex> wr_stack; //стек для write
        int i, index = 0, size = poliz.size();
        int a = 0;
        double d = 0;
        std::string s = "";
        std::string l;
        while (index < size){
            pc_el = poliz[index];
            switch(pc_el.get_type()){
                case LEX_NUM: case LEX_STR_CONST: case POLIZ_ADDRESS: case POLIZ_LABEL: //операнды
                    args.push(pc_el);
                    break;
                case LEX_ID:
                    i = pc_el.get_value(); //номер идентификатора в таблице
                    if (!TID[i].get_assign()) 
                        throw "POLIZ: INDEFINITE IDENTIFIER";
                    else{
                        if (TID[i].get_type() == LEX_INT || TID[i].get_type() == LEX_NUM){
                            args.push(Lex(LEX_NUM, TID[i].get_value()));
                            break;
                        }
                        else if (TID[i].get_type() == LEX_STRING || TID[i].get_type() == LEX_STR_CONST){
                            args.push(Lex(LEX_STR_CONST, 0, 0, TID[i].get_svalue()));
                            break;
                        }
                        else if (TID[i].get_type() == LEX_REAL || TID[i].get_type() == LEX_RNUM){
                            args.push(Lex(LEX_RNUM, 0, TID[i].get_dvalue()));
                            break;
                        }
                        else 
                            throw "POLIZ: IMPOSSIBLE IDENTIFIER TYPE";
                    }
                case POLIZ_GO:
                    from_poliz(args, o1);
                    index = o1.get_value() - 1;
                    break;
                case POLIZ_FGO:
                    from_poliz(args, o1);
                    from_poliz(args, o2); //логическое выражение
                    if (o2.get_type() != LEX_NUM) 
                        throw "INVALID !F CONDITION";
                    else {
                        if(o2.get_value() == 0) 
                            index = o1.get_value() - 1;
                        break;
                    }
                case LEX_NOT:
                    from_poliz(args, o1);
                    if (o1.get_value()){ //значение o1 не ноль 
                        o1.set_type(LEX_NUM);
                        o1.set_value(0);
                    }
                    else {
                        o1.set_type(LEX_NUM);
                        o1.set_value(1);
                    }
                    args.push(o1);
                    break;
                case LEX_OR:
                    from_poliz(args, o1);
                    from_poliz(args, o2);
                    o1.set_value(o2.get_value() || o1.get_value());
                    if (o1.get_value()) {
                        o1.set_type(LEX_NUM);
                        o1.set_value(1);
                    }
                    else {
                        o1.set_type(LEX_NUM);
                        o1.set_value(0);
                    }
                    args.push(o1);
                    break;
                case LEX_AND:
                    from_poliz(args, o1);
                    from_poliz(args, o2);
                    o1.set_value(o2.get_value() && o1.get_value());
                    if (o1.get_value()) {
                        o1.set_type(LEX_NUM);
                        o1.set_value(1);
                    }
                    else {
                        o1.set_type(LEX_NUM);
                        o1.set_value(0);
                    }
                    args.push(o1);
                    break;
                case LEX_PLUS: 
                    from_poliz(args, o1);
                    from_poliz(args, o2);
                    if (o1.get_type() == LEX_NUM ){
                        if(o2.get_type() == LEX_RNUM){
                            args.push(Lex(LEX_RNUM, 0, o2.get_dvalue() + o1.get_value()));
                        }else{
                            args.push(Lex(LEX_NUM, o2.get_value() + o1.get_value()));
                        }
                    }else if (o2.get_type() == LEX_RNUM){
                        args.push(Lex(LEX_RNUM, 0, o1.get_dvalue() + o2.get_dvalue()));
                    }
                    else if (o1.get_type() == LEX_STR_CONST)
                        args.push(Lex(LEX_STR_CONST, 0, 0, o1.get_svalue() + o1.get_svalue()));
                    break;
                case LEX_STAR:
                    from_poliz(args, o1);
                    from_poliz(args, o2);
                    if (o1.get_type() == LEX_NUM ){
                        if(o2.get_type() == LEX_RNUM){
                            args.push(Lex(LEX_RNUM, 0, o2.get_dvalue() * o1.get_value()));
                        }else{
                            args.push(Lex(LEX_NUM, o2.get_value() * o1.get_value()));
                        }
                    }else if (o2.get_type() == LEX_RNUM){
                        args.push(Lex(LEX_RNUM, 0, o1.get_dvalue() * o2.get_dvalue()));
                    }
                    break;
                case LEX_MINUS:
                    from_poliz(args, o1);
                    from_poliz(args, o2);
                    if (o1.get_type() == LEX_NUM ){
                        if(o2.get_type() == LEX_RNUM){
                            args.push(Lex(LEX_RNUM, 0, o2.get_dvalue() - o1.get_value()));
                        }else{
                            args.push(Lex(LEX_NUM, o2.get_value() - o1.get_value()));
                        }
                    }else if (o2.get_type() == LEX_RNUM){
                        args.push(Lex(LEX_RNUM, 0, o2.get_dvalue() - o1.get_dvalue()));
                    }
                    break;
                case LEX_SLASH:
                    from_poliz(args, o1);
                    from_poliz(args, o2);
                    if(o1.get_value())
                        if (o1.get_type() == LEX_NUM ){
                            if(o2.get_type() == LEX_RNUM){
                                args.push(Lex(LEX_RNUM, 0, o2.get_dvalue() / o1.get_value()));
                            }else{
                                args.push(Lex(LEX_NUM, o2.get_value() / o1.get_value()));
                            }
                        }else if (o2.get_type() == LEX_RNUM){
                            args.push(Lex(LEX_RNUM, 0, o2.get_dvalue() / o1.get_dvalue()));
                        }
                    else 
                        throw "POLIZ: DIVISION BY ZERO";
                    break;
                case LEX_DEQ:
                    from_poliz(args, o1);
                    from_poliz(args, o2);
                    if (o1.get_type() == LEX_NUM) {
                        if(o2.get_type() == LEX_RNUM) 
                            o1.set_value(o2.get_dvalue() == o1.get_value());
                        else 
                            o1.set_value(o2.get_value() == o1.get_value());
                    }
                    else if (o1.get_type() == LEX_RNUM) {
                        if (o2.get_type() == LEX_RNUM) {
                            o1.set_value(o2.get_dvalue() == o1.get_dvalue());
                        }
                            
                        else 
                            o1.set_value(o2.get_value() == o1.get_dvalue());
                    }
                    else if (o1.get_type() == LEX_STRING) {
                        o1.set_value(o1.get_svalue() == o2.get_svalue());
                    }

                    o1.set_type(LEX_NUM);
                    args.push(o1);
                    break;
                case LEX_NEQ:
                    from_poliz(args, o1);
                    from_poliz(args, o2);
                    if (o1.get_type() == LEX_NUM) {
                        if(o2.get_type() == LEX_RNUM) 
                            o1.set_value(o2.get_dvalue() != o1.get_value());
                        else 
                            o1.set_value(o2.get_value() != o1.get_value());
                    }
                    else if (o1.get_type() == LEX_RNUM) {
                        if (o2.get_type() == LEX_RNUM) 
                            o1.set_value(o2.get_dvalue() != o1.get_dvalue());
                        else 
                            o1.set_value(o2.get_value() != o1.get_dvalue());
                    }
                    else if (o1.get_type() == LEX_STRING) {
                        o1.set_value(o1.get_svalue() != o2.get_svalue());
                    }
                    o1.set_type(LEX_NUM);
                    args.push(o1);
                    break;
                case LEX_LTS:
                    from_poliz(args, o1);
                    from_poliz(args, o2);
                    if (o1.get_type() == LEX_NUM) {
                        if(o2.get_type() == LEX_RNUM) 
                            o1.set_value(o2.get_dvalue() < o1.get_value());
                        else 
                            o1.set_value(o2.get_value() < o1.get_value());
                    }
                    else if (o1.get_type() == LEX_RNUM) {
                        if (o2.get_type() == LEX_RNUM) 
                            o1.set_value(o2.get_dvalue() < o1.get_dvalue());
                        else 
                            o1.set_value(o2.get_value() < o1.get_dvalue());
                    }
                    else if (o1.get_type() == LEX_STRING) {
                        o1.set_value(o1.get_svalue() < o2.get_svalue());
                    }
                    o1.set_type(LEX_NUM); 
                    args.push(o1);
                    break;
                case LEX_GTS:
                    from_poliz(args, o1);
                    from_poliz(args, o2);
                    if (o1.get_type() == LEX_NUM) {
                        if(o2.get_type() == LEX_RNUM) 
                            o1.set_value(o2.get_dvalue() > o1.get_value());
                        else 
                            o1.set_value(o2.get_value() > o1.get_value());
                    }
                    else if (o1.get_type() == LEX_RNUM) {
                        if (o2.get_type() == LEX_RNUM) 
                            o1.set_value(o2.get_dvalue() > o1.get_dvalue());
                        else 
                            o1.set_value(o2.get_value() > o1.get_dvalue());
                    }
                    else if (o1.get_type() == LEX_STRING) {
                        o1.set_value(o1.get_svalue() > o2.get_svalue());
                    }
                    o1.set_type(LEX_NUM);
                    args.push(o1);
                    break;
                case LEX_LTSEQ:
                    from_poliz(args, o1);
                    from_poliz(args, o2);
                    if (o1.get_type() == LEX_NUM) {
                        if(o2.get_type() == LEX_RNUM) 
                            o1.set_value(o2.get_dvalue() <= o1.get_value());
                        else 
                            o1.set_value(o2.get_value() <= o1.get_value());
                    }
                    else if (o1.get_type() == LEX_RNUM) {
                        if (o2.get_type() == LEX_RNUM) 
                            o1.set_value(o2.get_dvalue() <= o1.get_dvalue());
                        else 
                            o1.set_value(o2.get_value() <= o1.get_dvalue());
                    }
                    o1.set_type(LEX_NUM);
                    args.push(o1);;
                    break;
                case LEX_GTSEQ:
                    from_poliz(args, o1);
                    from_poliz(args, o2);
                    if (o1.get_type() == LEX_NUM) {
                        if(o2.get_type() == LEX_RNUM) 
                            o1.set_value(o2.get_dvalue() >= o1.get_value());
                        else 
                            o1.set_value(o2.get_value() >= o1.get_value());
                    }
                    else if (o1.get_type() == LEX_RNUM) {
                        if (o2.get_type() == LEX_RNUM) 
                            o1.set_value(o2.get_dvalue() >= o1.get_dvalue());
                        else 
                            o1.set_value(o2.get_value() >= o1.get_dvalue());
                    }
                    o1.set_type(LEX_NUM);
                    args.push(o1);
                    break;
                case LEX_EQ:
                    from_poliz(args, o1);
                    from_poliz(args, o2);
                    i = o2.get_value();
                    if(TID[i].get_type() == LEX_INT) {
                        if(o1.get_type() == LEX_RNUM)
                            TID[i].set_value(floor(o1.get_dvalue()));
                        else 
                            TID[i].set_value(o1.get_value());
                    }
                    else if(TID[i].get_type() == LEX_REAL){
                        if(o1.get_type() == LEX_NUM)
                            TID[i].set_dvalue(o1.get_value());
                        else 
                            TID[i].set_dvalue(o1.get_dvalue());
                    } 
                    else if(TID[i].get_type() == LEX_STRING) 
                        TID[i].set_svalue(o1.get_svalue());
                    TID[i].set_assign();
                    args.push(o1);
                    break;
                case LEX_WRITE:
                    while(!args.empty()){
                        from_poliz(args, o1);
                        wr_stack.push(o1);
                    }
                    while(!wr_stack.empty()){
                        from_poliz(wr_stack, o1);
                        if (o1.get_type() == LEX_NUM) 
                            cout << o1.get_value()
                        ;
                        else if(o1.get_type() == LEX_RNUM) 
                            cout << o1.get_dvalue();
                        else if (o1.get_type() == LEX_STR_CONST) 
                            cout << o1.get_svalue();
                        cout << " ";
                    } 
                    cout << endl;
                    break;
                case LEX_READ:
                    from_poliz(args, o1);
                    i = o1.get_value();
                    if (TID[i].get_type() == LEX_INT){
                        cout << "Inset int value for " << TID[i].get_name() << " : ";
                        std::cin >> a;
                    }
                    else if (TID[i].get_type() == LEX_REAL){
                        cout << "Inset real value for " << TID[i].get_name() << " : ";
                        std::cin >> d;
                    }
                    else if (TID[i].get_type() == LEX_STRING){
                        cout << "Inset string value for " << TID[i].get_name() << " : ";
                        std::cin >> s;
                    }
                    TID[i].set_value(a);
                    TID[i].set_dvalue(d);
                    TID[i].set_svalue(s);
                    TID[i].set_assign();
                    a = 0;
                    d = 0;
                    s ="";
                    break;
                case LEX_SEMICOLON:
                    args.pop();
                    break;
                default:
                    throw "POLIZ: UNEXPECTED ELEMENT";
            }
            ++index;
        };
        // cout << "Execution finished!!!" << endl;
    }