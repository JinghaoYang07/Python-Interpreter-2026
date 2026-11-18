#pragma once
#include <cstdio>
#ifndef PYTHON_INTERPRETER_EVALVISITOR_H
#define PYTHON_INTERPRETER_EVALVISITOR_H


#include "Python3ParserBaseVisitor.h"
#include "int2048.h"
#include <map>
#include <unordered_map>
#include <cassert>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <utility>

using int2048 = sjtu::int2048;

class EvalVisitor : public Python3ParserBaseVisitor {
    struct ExprInfo {
        std::vector<std::vector<Python3Parser::TestContext*>> lists;
        std::vector<std::vector<int>> names;
        char op = 0;
    };
    std::unordered_map<Python3Parser::Expr_stmtContext*,ExprInfo> assign_cache;
    std::unordered_map<Python3Parser::And_testContext*,std::vector<Python3Parser::Not_testContext*>> and_cache;
    std::unordered_map<Python3Parser::Or_testContext*,std::vector<Python3Parser::And_testContext*>> or_cache;
	// TODO: override all methods of Python3ParserBaseVisitor
	std::unordered_map <std::string,int> symbol_ids;
	std::vector <std::any> globals;
	struct FCT {
        Python3Parser::ParametersContext *parameters;
        Python3Parser::SuiteContext *suite;
	};
	struct Args {
        int name;
		bool Initial;
		std::any copy;
	};
	std::unordered_map <std::string,std::vector<Args>> mp_args;
    std::unordered_map <std::string, FCT> mp_f;
	std::map<std::string,std::map<int,std::any>> tmp_mp;
	std::map <std::string, long long> tims;
	std::string generate_ID(std::string name) {
		long long nw = ++tims[name];
		std::string s = name, oth = "";
		s += '#';
        while(nw) {
			oth += nw % 10;
			nw /= 10;
		}
		for(int i = (int)oth.size() - 1; i >= 0; i--) {
			s += oth[i];
		}
		return s;
	}
	std::vector <std::string> stk;
	struct BreakSignal {};
	struct ContinueSignal {};
	std::unordered_map <Python3Parser::TermContext*, std::pair<std::vector<Python3Parser::FactorContext*>,std::vector<Python3Parser::Muldivmod_opContext*>>> term_cache;
	std::unordered_map <Python3Parser::Arith_exprContext*, std::pair<std::vector<Python3Parser::TermContext*>,std::vector<Python3Parser::Addorsub_opContext*>>> arith_cache;
    struct CompareInfo {
        std::vector<Python3Parser::Arith_exprContext*> exprs;
        std::vector<char> ops;
    };
    std::unordered_map<Python3Parser::ComparisonContext*,CompareInfo> compare_cache;
    std::unordered_map<Python3Parser::If_stmtContext*,std::pair<std::vector<Python3Parser::TestContext*>,std::vector<Python3Parser::SuiteContext*>>> if_cache;
    std::unordered_map<Python3Parser::FactorContext*,std::pair<antlr4::tree::ParseTree*,char>> factor_cache;

	std::unordered_map <antlr4::tree::ParseTree*, std::string> text_cache;
	struct VisitInfo {
        antlr4::tree::ParseTree *node;
        size_t rule;
    };
    std::unordered_map <antlr4::tree::ParseTree*,VisitInfo> node_cache;
	struct AtomInfo {
        int name = -1;
        antlr4::tree::ParseTree *expr = nullptr;
        std::any value = nullptr;
    };
	struct CallArg {
        int name;
        Python3Parser::TestContext *expr;
    };
	struct CallInfo {
        std::string name;
        std::vector<CallArg> args;
    };
	std::unordered_map <Python3Parser::AtomContext*,AtomInfo> atom_cache;
	std::unordered_map <Python3Parser::Atom_exprContext*,CallInfo> call_cache;
	std::unordered_map <antlr4::tree::ParseTree*,char> op_cache;
	int symbol(const std::string &name) {
        auto it = symbol_ids.find(name);
        if(it != symbol_ids.end()) return it->second;
        int id = globals.size();
        symbol_ids.emplace(name,id);
        globals.emplace_back();
        return id;
    }
	char operation(antlr4::tree::ParseTree *ctx) {
        auto it = op_cache.find(ctx);
        if(it != op_cache.end()) return it->second;
        const auto &s = node_text(ctx);
        char op = s[0];
        if(s == "//" || s == "//=") op = 'D';
        op_cache.emplace(ctx,op);
        return op;
    }
    struct SuiteInfo {
        Python3Parser::Simple_stmtContext *simple;
        std::vector<Python3Parser::StmtContext*> stmts;
    };
    std::unordered_map<Python3Parser::SuiteContext*,SuiteInfo> suite_cache;
	const std::string &node_text(antlr4::tree::ParseTree *ctx) {
		auto it = text_cache.find(ctx);
		if(it != text_cache.end()) return it->second;
		return text_cache.emplace(ctx,ctx->getText()).first->second;
	}
public:
	std::any visit(antlr4::tree::ParseTree *ctx) override {
		auto it = node_cache.find(ctx);
		if(it == node_cache.end()) {
			auto ptr = ctx;
			auto rule = dynamic_cast<antlr4::ParserRuleContext*>(ptr);
            it = node_cache.emplace(ctx,VisitInfo{ptr,rule ? rule->getRuleIndex() : size_t(-1)}).first;
		}
        auto ptr = it->second.node;
        switch(it->second.rule) {
            case Python3Parser::RuleAtom: return visitAtom(static_cast<Python3Parser::AtomContext*>(ptr));
            case Python3Parser::RuleAtom_expr: return visitAtom_expr(static_cast<Python3Parser::Atom_exprContext*>(ptr));
            case Python3Parser::RuleFactor: return visitFactor(static_cast<Python3Parser::FactorContext*>(ptr));
            case Python3Parser::RuleTerm: return visitTerm(static_cast<Python3Parser::TermContext*>(ptr));
            case Python3Parser::RuleArith_expr: return visitArith_expr(static_cast<Python3Parser::Arith_exprContext*>(ptr));
            case Python3Parser::RuleComparison: return visitComparison(static_cast<Python3Parser::ComparisonContext*>(ptr));
            case Python3Parser::RuleNot_test: return visitNot_test(static_cast<Python3Parser::Not_testContext*>(ptr));
            case Python3Parser::RuleAnd_test: return visitAnd_test(static_cast<Python3Parser::And_testContext*>(ptr));
            case Python3Parser::RuleOr_test: return visitOr_test(static_cast<Python3Parser::Or_testContext*>(ptr));
            case Python3Parser::RuleExpr_stmt: return visitExpr_stmt(static_cast<Python3Parser::Expr_stmtContext*>(ptr));
            case Python3Parser::RuleIf_stmt: return visitIf_stmt(static_cast<Python3Parser::If_stmtContext*>(ptr));
            case Python3Parser::RuleWhile_stmt: return visitWhile_stmt(static_cast<Python3Parser::While_stmtContext*>(ptr));
            case Python3Parser::RuleSuite: return visitSuite(static_cast<Python3Parser::SuiteContext*>(ptr));
            case Python3Parser::RuleFuncdef: return visitFuncdef(static_cast<Python3Parser::FuncdefContext*>(ptr));
            case Python3Parser::RuleReturn_stmt: return visitReturn_stmt(static_cast<Python3Parser::Return_stmtContext*>(ptr));
            case Python3Parser::RuleBreak_stmt: return visitBreak_stmt(static_cast<Python3Parser::Break_stmtContext*>(ptr));
            case Python3Parser::RuleContinue_stmt: return visitContinue_stmt(static_cast<Python3Parser::Continue_stmtContext*>(ptr));
            case Python3Parser::RuleTestlist: return visitTestlist(static_cast<Python3Parser::TestlistContext*>(ptr));
            case Python3Parser::RuleFormat_string: return visitFormat_string(static_cast<Python3Parser::Format_stringContext*>(ptr));
            default: return Python3ParserBaseVisitor::visit(ptr);
        }
	}
private:
	bool returning = false;
	std::any return_value = nullptr;
	bool isNone(const std::any &x) {
		return std::any_cast <std::nullptr_t> (&x) != nullptr;
	}
	bool isString(const std::any &x) {
		return std::any_cast <std::string> (&x) != nullptr;
	}
	bool isInt2048(const std::any &x) {
		return std::any_cast <int2048> (&x) != nullptr;
	}
	bool isDouble(const std::any &x) {
		return std::any_cast <double> (&x) != nullptr;
	}
	bool isBool(const std::any &x) {
		return std::any_cast <bool> (&x) != nullptr;
	}
	std::string toString(const std::any &x) {
		if(isString(x)) {
			return std::any_cast <const std::string&> (x);
		}
		if(isBool(x)) {
			if(toBool(x) == true) {
				return "True";
			}
			return "False";
		}
		if(isNone(x)) {
			return "None";
		}
		if(isDouble(x)) {
			return magic_get(toDouble(x));
		}
		if(isInt2048(x)) {
			return change_string(std::any_cast <const int2048&> (x));
		}
		assert(0);
	}
	double toDouble(const std::any &x) {
        if(std::any_cast <double> (&x)) {
			return std::any_cast <double> (x);
		}
		if(std::any_cast <int2048> (&x)) {
			return change_double(std::any_cast <const int2048&> (x));
		}
		if(std::any_cast <bool> (&x)) {
			if(std::any_cast <bool> (x)) return 1.0;
			return 0.0;
		}
		if(isString(x)) {
            std::string s = toString(x);;
			return std::stod(s);
		}
		assert(0);
	}
	const int2048 &integer_value(const std::any &x) {
		if(auto ptr = std::any_cast <int2048> (&x)) return *ptr;
		static const int2048 zero(0), one(1);
		return std::any_cast <bool> (x) ? one : zero;
	}
	int2048 toInt2048(const std::any &x) {
		if(std::any_cast <int2048> (&x)) {
			return std::any_cast <const int2048&> (x);
		} 
		if(std::any_cast <bool> (&x)) {
			return (int2048)(std::any_cast <bool> (x));
		}
		if(isString(x)) {
			return int2048(toString(x));
		}
		if(isDouble(x)) {
            return int2048((long long)(toDouble(x)));
		}
		assert(0);
	}
	std::nullptr_t toNone(const std::any &x) {
		return std::any_cast <std::nullptr_t> (x);
	}
	std::string magic_get(const double &s) {
        std::ostringstream out;
		out << std::fixed << std::setprecision(6) << s;
		return out.str();
	}
	bool toBool(const std::any &x) {
		if(isBool(x)) {
			return std::any_cast <bool> (x);
		}
		if(isString(x)) {
			return !std::any_cast <const std::string&> (x).empty();
		}
		if(isNone(x)) {
			return false;
		}
		if(isDouble(x)) {
			return toDouble(x) != 0.0;
		}
		if(isInt2048(x)) {
			return !std::any_cast <const int2048&> (x).isZero();
		}
		assert(0);
	}
	std::any query_value(int key) {
        if(!stk.empty()) {
            auto it = tmp_mp[stk.back()].find(key);
            if(it != tmp_mp[stk.back()].end()) return it->second;
        }
        assert(globals[key].has_value());
        return globals[key];
    }
	void change_value(int key,std::any val) {
        if(stk.empty()) {
            globals[key] = std::move(val);
            return;
        }
        auto it = tmp_mp[stk.back()].find(key);
        if(it != tmp_mp[stk.back()].end()) {
            it->second = std::move(val);
            return;
        }
        if(globals[key].has_value()) {
            globals[key] = std::move(val);
            return;
        }
        tmp_mp[stk.back()][key] = std::move(val);
    }
	std::string deal_string(std::string in) {
		std::string ret = "";
		for(int i = 1; i < (int)(in.size()) - 1; i++) {
			if(in[i] != '\\') {
				ret += in[i];
			}
			else if(in[i+1] == '\\') {
				ret += '\\';
			    i++;
			}
			else if(in[i+1] == '\'') {
				ret += '\'';
				i++;
			}
			else if(in[i+1] == '?') {
				ret += '?';
				i++;
			}
			else if(in[i+1] == 'n') {
				ret += '\n';
				i++;
			}
            else if(in[i+1] == 't') {
				ret += '\t';
				i++;
			}
			else if(in[i+1] == 'r') {
				ret += '\r';
				i++;
			}
			else if(in[i+1] == 'f') {
				ret += '\f';
				i++;
			}
			else if(in[i+1] == 'v') {
				ret += '\v';
				i++;
			}
		}
		return ret;
	}
	std::string deal_fstring(std::string in) {
		std::string ret = "";
		for(int i = 0; i < (int)(in.size()); i++) {
			if(in[i] != '\\') {
				if(in[i] == '{') {
					ret += '{';
					i++;
				}
				else if(in[i] == '}') {
					ret += '}';
					i++;
				}
				else {
					ret += in[i];
				}
			}
			else if(in[i+1] == '\\') {
				ret += '\\';
			    i++;
			}
			else if(in[i+1] == '\'') {
				ret += '\'';
				i++;
			}
			else if(in[i+1] == '?') {
				ret += '?';
				i++;
			}
			else if(in[i+1] == 'n') {
				ret += '\n';
				i++;
			}
            else if(in[i+1] == 't') {
				ret += '\t';
				i++;
			}
			else if(in[i+1] == 'r') {
				ret += '\r';
				i++;
			}
			else if(in[i+1] == 'f') {
				ret += '\f';
				i++;
			}
			else if(in[i+1] == 'v') {
				ret += '\v';
				i++;
			}
			
		}
		return ret;
	}
	std::any visitAtom(Python3Parser::AtomContext *ctx) override {
        auto it = atom_cache.find(ctx);
        if(it == atom_cache.end()) {
            AtomInfo info;
            if(auto ptr = ctx->NAME()) info.name = symbol(node_text(ptr));
            else if(auto ptr = ctx->NUMBER()) {
                const auto &s = node_text(ptr);
                if(s.find('.') != std::string::npos) info.value = std::stod(s);
                else info.value = int2048(s);
            }
            else if(ctx->NONE()) info.value = nullptr;
            else if(ctx->TRUE()) info.value = true;
            else if(ctx->FALSE()) info.value = false;
            else if(auto ptr = ctx->test()) info.expr = ptr;
            else if(auto ptr = ctx->format_string()) info.expr = ptr;
            else {
                std::string ret;
                for(auto ptr : ctx->STRING()) ret += deal_string(node_text(ptr));
                info.value = std::move(ret);
            }
            it = atom_cache.emplace(ctx,std::move(info)).first;
        }
        if(it->second.name != -1) return query_value(it->second.name);
        if(it->second.expr) return visit(it->second.expr);
        return it->second.value;
    }
	std::any visitAtom_expr(Python3Parser::Atom_exprContext *ctx) override {
        auto it = call_cache.find(ctx);
        if(it == call_cache.end()) {
            auto trailer = ctx->trailer();
            if(!trailer) return visit(ctx->atom());
            CallInfo info;
            info.name = node_text(ctx->atom()->NAME());
            if(auto args = trailer->arglist()) {
                for(auto arg : args->argument()) {
                    if(arg->ASSIGN()) info.args.push_back({symbol(node_text(arg->test(0))),arg->test(1)});
                    else info.args.push_back({-1,arg->test(0)});
                }
            }
            it = call_cache.emplace(ctx,std::move(info)).first;
        }
        const auto &s = it->second.name;
        const auto &args = it->second.args;
        if(s == "print") {
            bool flg = false;
            for(const auto &arg : args) {
                if(flg) std::cout << ' ';
                flg = true;
                auto out = visit(arg.expr);
                if(auto ptr = std::any_cast<int2048>(&out)) std::cout << *ptr;
                else if(auto ptr = std::any_cast<double>(&out)) printf("%.6lf",*ptr);
                else if(auto ptr = std::any_cast<bool>(&out)) std::cout << (*ptr ? "True" : "False");
                else if(auto ptr = std::any_cast<std::string>(&out)) std::cout << *ptr;
                else std::cout << "None";
            }
            std::cout << '\n';
            return nullptr;
        }
        if(s == "str") return toString(visit(args[0].expr));
        if(s == "int") return toInt2048(visit(args[0].expr));
        if(s == "float") return toDouble(visit(args[0].expr));
        if(s == "bool") return toBool(visit(args[0].expr));
        FCT f = mp_f.at(s);
        const auto &params = mp_args.at(s);
        std::string real_name = generate_ID(s);
        int num = 0;
        for(const auto &arg : args) {
            auto value = visit(arg.expr);
            int name = arg.name == -1 ? params[num++].name : arg.name;
            tmp_mp[real_name][name] = std::move(value);
        }
        for(int i = num; i < (int)params.size(); i++) {
            if(tmp_mp[real_name].find(params[i].name) == tmp_mp[real_name].end()) tmp_mp[real_name][params[i].name] = params[i].copy;
        }
        stk.push_back(real_name);
        visit(f.suite);
        std::any ans = returning ? std::move(return_value) : std::any(nullptr);
        returning = false;
        return_value = nullptr;
        tmp_mp.erase(real_name);
        stk.pop_back();
        return ans;
    }
	std::any visitFuncdef(Python3Parser::FuncdefContext *ctx) override {
        std::string name = node_text(ctx->NAME());
		mp_f[name].parameters = ctx->parameters();
		mp_f[name].suite = ctx->suite();
		mp_args[name].clear();
		auto args = ctx->parameters()->typedargslist();
		int all = (args == nullptr ? 0 : (args->tfpdef()).size());
		int init = (all == 0 ? 0 : (args->test()).size()), pos = 0;
		for(int i = 0; i < all; i++) {
			if(i < all - init) {
                mp_args[name].push_back({symbol(node_text(args->tfpdef(i)->NAME())), false, nullptr});
			}
			else {
                mp_args[name].push_back({symbol(node_text(args->tfpdef(i)->NAME())), true, visit(args->test(pos++))});
			}
		}
		return nullptr;
    }
    std::any visitFactor(Python3Parser::FactorContext *ctx) override {
        auto it = factor_cache.find(ctx);
        if(it == factor_cache.end()) {
            antlr4::tree::ParseTree *expr = ctx->factor();
            if(!expr) expr = ctx->atom_expr();
            char op = ctx->MINUS() ? '-' : (ctx->ADD() ? '+' : 0);
            it = factor_cache.emplace(ctx,std::make_pair(expr,op)).first;
        }
        auto info = it->second;
        auto ret = visit(info.first);
        if(info.second && isBool(ret)) ret = int2048(std::any_cast<bool>(ret));
        if(info.second == '-') {
            if(auto ptr = std::any_cast<int2048>(&ret)) *ptr = -*ptr;
            else ret = -std::any_cast<double>(ret);
        }
        return ret;
    }
	std::any visitTerm(Python3Parser::TermContext *ctx) override {
		auto it = term_cache.find(ctx);
		if(it == term_cache.end()) it = term_cache.emplace(ctx,std::make_pair(ctx->factor(),ctx->muldivmod_op())).first;
		const auto &factors = it->second.first;
		const auto &ops = it->second.second;
		auto tmp = visit(factors[0]);
		if((int)(factors.size()) == 1) return tmp;
		if(std::any_cast <bool> (&tmp)) tmp = int2048(std::any_cast <bool> (tmp)); 
		bool double_flg = (std::any_cast <double> (&tmp) != nullptr);
        int2048 ret_ll(0); 
		double ret_db = 0.0;
		bool sp_flag=false;
		std::string nt,ret="";
		if(std::any_cast <std::string> (&tmp)) {
			sp_flag = true;
			nt = ret = std::any_cast <const std::string&> (tmp);
		}
		else {
			if(!double_flg) {
				ret_ll = std::move(std::any_cast <int2048&> (tmp));
			}
	    	else {
				ret_db = std::any_cast <double> (tmp);
			}
		}
		for(int i = 1; i < (int)(factors.size()); i++) {
			auto A = visit(factors[i]);
            char op = operation(ops[i-1]);
			if(std::any_cast <bool> (&A)) A = int2048(std::any_cast <bool> (A)); 
			if(std::any_cast <std::string> (&A)) {
				sp_flag = true;
				nt = std::any_cast <const std::string&> (A);
                if(nt != "") {
					for(int j = 1;j <= change_ll(ret_ll);j++) {
			    	    ret += nt;
			        }
				}
				continue;
			}
			else if(sp_flag) {
				long long tms=change_ll(std::any_cast <const int2048&> (A));
                if(tms <= 0 || ret == "") {
					ret = "";
				}
				else {
					nt = ret, ret = "";
					while(tms--) {
						ret += nt;
					}
				}
				continue;
			}
			if(std::any_cast <int2048> (&A)) {
                if(double_flg) {
                    if(op == '*') {
						ret_db *= change_double(std::any_cast <const int2048&> (A));
					}
					else if(op == '/') {
						int2048 nw = std::any_cast <const int2048&> (A);
						ret_db /= change_double(nw);
					}
					else if(op == 'D') {
                        ret_db /= change_double(std::any_cast <const int2048&> (A));
						ret_db = std::floor(ret_db);
					}
					else {
						double dv = change_double(std::any_cast <const int2048&> (A));
						double tmp = std::floor(ret_db / dv);
						ret_db -= tmp * dv;
					}
				}
				else {
					if(op == '*') {
						ret_ll *= std::any_cast <const int2048&> (A);
					}
					else if(op == '/') {
						int2048 nw = std::any_cast <const int2048&> (A);
					    double_flg = true;
						ret_db = change_double(ret_ll) / change_double(nw);
					}
					else if(op == 'D') {
                        ret_ll /= std::any_cast <const int2048&> (A);
					}
					else {
						ret_ll %= std::any_cast <const int2048&> (A);
					}
				}
			}
			else {
                if(!double_flg) {
					ret_db = change_double(ret_ll);
					double_flg = true;
				}
				if(op == '*') {
     				ret_db *= std::any_cast <double> (A);
				}
				else if(op == '/') {
					auto nw = std::any_cast <double> (A);
					ret_db /= nw;
				}
				else if(op == 'D') {
                    ret_db = std::floor(ret_db / std::any_cast <double> (A));
				}
				else {
					double dv = std::any_cast <double> (A);
					double tmp = std::floor(ret_db / dv);
					ret_db -= tmp * dv;
				}
			}
		}
		if(sp_flag) {
			return ret;
		}
		if(double_flg) {
			return ret_db;
		}
		return ret_ll;
	}
	std::any visitArith_expr(Python3Parser::Arith_exprContext *ctx) override {
		auto it = arith_cache.find(ctx);
		if(it == arith_cache.end()) it = arith_cache.emplace(ctx,std::make_pair(ctx->term(),ctx->addorsub_op())).first;
		const auto &terms = it->second.first;
		const auto &ops = it->second.second;
	    auto tmp = visit(terms[0]);
		if((int)(terms.size()) == 1) return tmp;
		if(std::any_cast <bool> (&tmp)) tmp = int2048(std::any_cast <bool> (tmp)); 
		bool double_flg = (std::any_cast <double> (&tmp) != nullptr);
        int2048 ret_ll(0); 
		double ret_db = 0.0;
		std::string sp_ret="";
		bool sp_check = false;
		if(std::any_cast <std::string> (&tmp)) {
			sp_ret += std::any_cast <const std::string&> (tmp);
			sp_check = true;
		}
		else {
			if(!double_flg) ret_ll = std::move(std::any_cast <int2048&> (tmp));
		    else ret_db = std::any_cast <double> (tmp);
		}
		for(int i = 1; i < (int)(terms.size()); i++) {
			auto A = visit(terms[i]);
            char op = operation(ops[i-1]);
			if(std::any_cast <std::string> (&A)) {
                sp_ret += std::any_cast <const std::string&> (A);
				sp_check = true;
				continue;
			}
			if(std::any_cast <bool> (&A)) A = int2048(std::any_cast <bool> (A)); 
			if(std::any_cast <int2048> (&A)) {
                if(double_flg) {
                    if(op == '+') {
						ret_db += change_double(std::any_cast <const int2048&> (A));
					}
					else {
						ret_db -= change_double(std::any_cast <const int2048&> (A));
					}
				}
				else {
					if(op == '+') {
						ret_ll += std::any_cast <const int2048&> (A);
					}
					else {
						ret_ll -= std::any_cast <const int2048&> (A);
					}
				}
			}
			else {
                if(!double_flg) {
					ret_db = change_double(ret_ll);
					double_flg = true;
				}
				if(op == '+') {
					ret_db += std::any_cast <double> (A);
				}
				else {
					ret_db -= std::any_cast <double> (A);
				}
			}
		}
		if(sp_check) {
			return sp_ret;
		}
		if(double_flg) {
			return ret_db;
		}
		return ret_ll;
	}
	std::any binary_value(std::any lhs,const std::any &rhs,char op) {
        if(isString(lhs) || isString(rhs)) {
            if(op == '+') {
                auto &ret = std::any_cast<std::string&>(lhs);
                ret += std::any_cast<const std::string&>(rhs);
                return lhs;
            }
            const auto &text = isString(lhs) ? std::any_cast<const std::string&>(lhs) : std::any_cast<const std::string&>(rhs);
            long long times = change_ll(integer_value(isString(lhs) ? rhs : lhs));
            std::string ret;
            if(times > 0 && !text.empty()) {
                ret.reserve(text.size()*times);
                while(times--) ret += text;
            }
            return ret;
        }
        if(op == '/' || isDouble(lhs) || isDouble(rhs)) {
            double a = toDouble(lhs),b = toDouble(rhs);
            if(op == '+') return a+b;
            if(op == '-') return a-b;
            if(op == '*') return a*b;
            if(op == '/') return a/b;
            if(op == 'D') return std::floor(a/b);
            return a-std::floor(a/b)*b;
        }
        if(isBool(lhs)) lhs = int2048(std::any_cast<bool>(lhs));
        auto &a = std::any_cast<int2048&>(lhs);
        const auto &b = integer_value(rhs);
        if(op == '+') a += b;
        else if(op == '-') a -= b;
        else if(op == '*') a *= b;
        else if(op == 'D') a /= b;
        else a %= b;
        return lhs;
    }
	std::any visitExpr_stmt(Python3Parser::Expr_stmtContext *ctx) override {
        auto it = assign_cache.find(ctx);
        if(it == assign_cache.end()) {
            ExprInfo info;
            for(auto ptr : ctx->testlist()) info.lists.push_back(ptr->test());
            if(auto ptr = ctx->augassign()) info.op = operation(ptr);
            for(int i = 0; i+1 < (int)info.lists.size(); i++) {
                info.names.emplace_back();
                for(auto ptr : info.lists[i]) info.names.back().push_back(symbol(node_text(ptr)));
            }
            it = assign_cache.emplace(ctx,std::move(info)).first;
        }
        const auto &info = it->second;
        const auto &lists = info.lists;
        if(lists.size() == 1) {
            visit(lists[0][0]);
            return nullptr;
        }
        if(info.names[0].size() == 1 && lists.back().size() == 1) {
            std::any left;
            if(info.op) left = query_value(info.names[0][0]);
            auto right = visit(lists.back()[0]);
            if(auto ptr = std::any_cast<std::vector<std::any>>(&right)) {
                auto value = std::move(ptr->at(0));
                right = std::move(value);
            }
            if(info.op) change_value(info.names[0][0],binary_value(std::move(left),right,info.op));
            else {
                for(const auto &names : info.names) change_value(names[0],right);
            }
            return nullptr;
        }
        std::vector<std::any> left,right;
        if(info.op) {
            left.reserve(info.names[0].size());
            for(int name : info.names[0]) left.push_back(query_value(name));
        }
        auto first = visit(lists.back()[0]);
        if(auto ptr = std::any_cast<std::vector<std::any>>(&first)) right = std::move(*ptr);
        else {
            right.reserve(lists.back().size());
            right.push_back(std::move(first));
            for(int i = 1; i < (int)lists.back().size(); i++) right.push_back(visit(lists.back()[i]));
        }
        if(info.op) {
            for(int i = 0; i < (int)left.size(); i++) change_value(info.names[0][i],binary_value(std::move(left[i]),right[i],info.op));
        }
        else {
            for(const auto &names : info.names) {
                for(int i = 0; i < (int)names.size(); i++) change_value(names[i],right[i]);
            }
        }
        return nullptr;
    }
    template<class T> bool compare_value(const T &a,const T &b,char op) {
        if(op == '=') return a == b;
        if(op == '!') return a != b;
        if(op == '<') return a < b;
        if(op == '>') return a > b;
        if(op == 'l') return a <= b;
        return a >= b;
    }
    bool compare_value(const std::any &lhs,const std::any &rhs,char op) {
        if(isNone(lhs) || isNone(rhs)) return isNone(lhs) && isNone(rhs) && op == '=';
        if(isString(lhs) || isString(rhs)) {
            if(!isString(lhs) || !isString(rhs)) return false;
            return compare_value(std::any_cast<const std::string&>(lhs),std::any_cast<const std::string&>(rhs),op);
        }
        if(isDouble(lhs) || isDouble(rhs)) return compare_value(toDouble(lhs),toDouble(rhs),op);
        return compare_value(integer_value(lhs),integer_value(rhs),op);
    }
    std::any visitComparison(Python3Parser::ComparisonContext *ctx) override {
        auto it = compare_cache.find(ctx);
        if(it == compare_cache.end()) {
            CompareInfo info;
            info.exprs = ctx->arith_expr();
            for(auto ptr : ctx->comp_op()) {
                const auto &s = node_text(ptr);
                char op = s[0];
                if(s == "<=") op = 'l';
                if(s == ">=") op = 'g';
                info.ops.push_back(op);
            }
            it = compare_cache.emplace(ctx,std::move(info)).first;
        }
        const auto &exprs = it->second.exprs;
        const auto &ops = it->second.ops;
        auto lhs = visit(exprs[0]);
        if(ops.empty()) return lhs;
        for(int i = 0; i < (int)ops.size(); i++) {
            auto rhs = visit(exprs[i+1]);
            if(!compare_value(lhs,rhs,ops[i])) return false;
            lhs = std::move(rhs);
        }
        return true;
    }
	std::any visitNot_test(Python3Parser::Not_testContext *ctx) override {
		if(ctx->not_test()) {
			return !toBool(visit(ctx->not_test()));
		}
        return visit(ctx->comparison());
	}
	std::any visitAnd_test(Python3Parser::And_testContext *ctx) override {
        auto it = and_cache.find(ctx);
        if(it == and_cache.end()) it = and_cache.emplace(ctx,ctx->not_test()).first;
        const auto &nodes = it->second;

		if(nodes.size() == 1) {
			return visit(nodes[0]);
		}
        for(auto ptr : nodes) {
			bool ret = toBool(visit(ptr));
			if(!ret) {
				return false;
			}
		}
		return true;
    }
	std::any visitOr_test(Python3Parser::Or_testContext *ctx) override {
        auto it = or_cache.find(ctx);
        if(it == or_cache.end()) it = or_cache.emplace(ctx,ctx->and_test()).first;
        const auto &nodes = it->second;

		if(nodes.size() == 1) {
			return visit(nodes[0]);
		}
		for(auto ptr : nodes) {
			bool ret = toBool(visit(ptr));
			if(ret) {
				return true;
			}
		}
		return false;
	}
    std::any visitIf_stmt(Python3Parser::If_stmtContext *ctx) override {
        auto it = if_cache.find(ctx);
        if(it == if_cache.end()) it = if_cache.emplace(ctx,std::make_pair(ctx->test(),ctx->suite())).first;
        const auto &tests = it->second.first;
        const auto &suites = it->second.second;
        for(int i = 0; i < (int)tests.size(); i++) {
            if(toBool(visit(tests[i]))) {
                visit(suites[i]);
                return nullptr;
            }
        }
        if(suites.size() > tests.size()) visit(suites.back());
        return nullptr;
    }
    std::any visitSuite(Python3Parser::SuiteContext *ctx) override {
        auto it = suite_cache.find(ctx);
        if(it == suite_cache.end()) it = suite_cache.emplace(ctx,SuiteInfo{ctx->simple_stmt(),ctx->stmt()}).first;
        if(it->second.simple) return visit(it->second.simple);
        const auto &stmts = it->second.stmts;
        for(auto stmt : stmts) {
            visit(stmt);
            if(returning) break;
        }
        return nullptr;
    }
	std::any visitWhile_stmt(Python3Parser::While_stmtContext *ctx) override {
        auto test = ctx->test();
        auto suite = ctx->suite();
        while(toBool(visit(test))) {
            try {
                visit(suite);
                if(returning) break;
            }
            catch(const BreakSignal &) {
                break;
            }
            catch(const ContinueSignal &) {
                continue;
            }
        }
        return nullptr;
    }
    std::any visitBreak_stmt(Python3Parser::Break_stmtContext *ctx) override {
        throw BreakSignal{};
    }
    std::any visitContinue_stmt(Python3Parser::Continue_stmtContext *ctx) override {
        throw ContinueSignal{};
    }
	std::any visitTestlist(Python3Parser::TestlistContext *ctx) override {
		std::vector <std::any> ret{};
		int siz = (ctx->test()).size();
		for(int i = 0; i < siz; i++) {
			ret.push_back(visit(ctx->test(i)));
		}
		return ret;
	}
    std::any visitReturn_stmt(Python3Parser::Return_stmtContext *ctx) override {
		std::any value = nullptr;
		if(ctx->testlist()) {
			if(ctx->testlist()->test().size() == 1) {
				value = visit(ctx->testlist()->test(0));
			}
			else {
				value = visit(ctx->testlist());
			}
		}
		return_value = std::move(value);
		returning = true;
		return nullptr;
    }

	std::any visitFormat_string(Python3Parser::Format_stringContext *ctx) override {
		std::string ret = "";
        for(auto child : ctx->children) {
			if(auto LIST = dynamic_cast <Python3Parser::TestlistContext*> (child)) {
                auto tmp = visit(LIST->test(0));
				ret += toString(tmp);
			}
			else if(auto TEXT = dynamic_cast <antlr4::tree::TerminalNode*> (child)) {
                if(TEXT->getSymbol()->getType() == Python3Parser::FORMAT_STRING_LITERAL) {
                    ret += deal_fstring(node_text(TEXT));
				}
			}
		}
		return ret;
    }
};


#endif//PYTHON_INTERPRETER_EVALVISITOR_H
