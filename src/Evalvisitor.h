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
    std::unordered_map<Python3Parser::Expr_stmtContext*,std::vector<std::vector<Python3Parser::TestContext*>>> assign_cache;
    std::unordered_map<Python3Parser::And_testContext*,std::vector<Python3Parser::Not_testContext*>> and_cache;
    std::unordered_map<Python3Parser::Or_testContext*,std::vector<Python3Parser::And_testContext*>> or_cache;
	// TODO: override all methods of Python3ParserBaseVisitor
	std::map <std::string, std::any> mp;
	struct FCT {
        Python3Parser::ParametersContext *parameters;
        Python3Parser::SuiteContext *suite;
	};
	struct Args {
        std::string name;
		bool Initial;
		std::any copy;
	};
	std::map <std::string,std::vector<Args>> mp_args;
    std::map <std::string, FCT> mp_f;
	std::map <std::string, std::map <std::string, std::any>> tmp_mp;
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
	std::unordered_map <Python3Parser::ComparisonContext*, std::pair<std::vector<Python3Parser::Arith_exprContext*>,std::vector<Python3Parser::Comp_opContext*>>> compare_cache;

	std::unordered_map <antlr4::tree::ParseTree*, std::string> text_cache;
	std::unordered_map <Python3Parser::AtomContext*, std::any> literal_cache;
	std::unordered_map <Python3Parser::SuiteContext*, std::vector<Python3Parser::StmtContext*>> suite_cache;
	const std::string &node_text(antlr4::tree::ParseTree *ctx) {
		auto it = text_cache.find(ctx);
		if(it != text_cache.end()) return it->second;
		return text_cache.emplace(ctx,ctx->getText()).first->second;
	}
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
	std::any query_value(const std::string &key) {
		if(stk.empty()) {
			return mp.at(key);
		}
		auto it = tmp_mp[stk.back()].find(key);
		if(it != tmp_mp[stk.back()].end()) {
			return it->second;
		}
		return mp.at(key);
	}
	void change_value(const std::string &key, std::any val) {
		if(stk.empty()) {
			mp[key] = std::move(val);
		}
		else {
            auto it = tmp_mp[stk.back()].find(key);
		    if(it != tmp_mp[stk.back()].end()) {
			    it->second = std::move(val);
				return ;
		    }
			auto itt = mp.find(key);
		    if(itt != mp.end()) {
			    itt->second = std::move(val);
				return ;
		    }
		    tmp_mp[stk.back()][key] = std::move(val);
		}
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
		auto it = literal_cache.find(ctx);
		if(it != literal_cache.end()) return it->second;
		if(ctx->NAME()) {
            //变量赋值？
			const std::string &s = node_text(ctx->NAME());
			return query_value(s);
		}
		else if(ctx->NUMBER()) {
            const std::string &s = node_text(ctx->NUMBER());
			bool fd = false;
			for(int i = 0; i < (int)(s.size()); i++) {
				if(s[i] == '.') {
					fd = true;
					break;
				}
			}
			if(fd) return literal_cache.emplace(ctx,std::stod(s)).first->second;
			return literal_cache.emplace(ctx,int2048(s)).first->second;
		}
		else if(ctx->NONE()) {
            return nullptr;
		}
		else if(ctx->TRUE()) {
            return true;
		}
		else if(ctx->FALSE()) {
            return false;
		}
		else if(ctx->test()) {
            return visit(ctx->test());
		}
		else if(ctx->format_string()) {
			return visit(ctx->format_string());
		}
		std::string ret = "";
		for(int i = 0; i < (int)(ctx->STRING().size()); i++) {
			ret += deal_string(node_text(ctx->STRING(i)));
		}
		return literal_cache.emplace(ctx,std::move(ret)).first->second;
	}
	std::any visitAtom_expr(Python3Parser::Atom_exprContext *ctx) override {
		if(ctx->trailer() == nullptr) {
			return visit(ctx->atom());
		}
		const std::string &s = node_text(ctx->atom()->NAME());
		if(s == "print") {
			auto args = ctx->trailer()->arglist();
			if(args == nullptr) {
                std::cout<< '\n';
				return nullptr;
			}
			bool flg = false;
			for(auto nw:args->argument()) {
				if(flg) {
					std::cout << " ";
				}
				else {
					flg = true;
				}
				std::any out = visit(nw);
				if(auto p = std::any_cast <int2048> (&out)) {
					std::cout << (*p);
				}
				else if(auto p = std::any_cast <double> (&out)) {
					printf("%.6lf", (*p));
				}
				else if(auto p = std::any_cast <bool> (&out)) {
					if((*p) == true) {
						std::cout << "True";
					}
					else {
						std::cout << "False";
					}
				}
				else if(auto p = std::any_cast <std::string> (&out)) {
					std::cout << (*p);
				}
				else {
					std::cout << "None";
				}
			}
			std::cout << '\n';
			return nullptr;
		}
		else if (s == "str") {
			auto ret = visit(ctx->trailer()->arglist()->argument(0)->test(0));
			return toString(ret);
		}
		else if (s == "int") {
            auto ret = visit(ctx->trailer()->arglist()->argument(0)->test(0));
			return toInt2048(ret);
		}
		else if (s == "float") {
            auto ret = visit(ctx->trailer()->arglist()->argument(0)->test(0));
			return toDouble(ret);
		}
		else if (s == "bool") {
            auto ret = visit(ctx->trailer()->arglist()->argument(0)->test(0));
			return toBool(ret);
		}
		else if (s != "") {
            FCT f = mp_f[s];
			auto ptr = f.suite;
			std::string real_name = generate_ID(s);
			auto args = (ctx->trailer()->arglist());
			int siz = (args == nullptr ? 0 : (ctx->trailer()->arglist()->argument()).size());
			int all = (mp_args[s].size()), num = 0;
			for(int i = 0; i < siz; i++) {
				if(args->argument(i)->ASSIGN() == nullptr) {
					std::any value = visit(args->argument(i)->test(0));
					tmp_mp[real_name][mp_args[s][i].name] = std::move(value);
					num++;
				}
				else {
					std::any value = visit(args->argument(i)->test(1));
					tmp_mp[real_name][node_text(args->argument(i)->test(0))] = std::move(value);
				}
			}
			for(int i = num; i < all; i++) {
				std::string name = mp_args[s][i].name;
				auto it = tmp_mp[real_name].find(name);
				if(it == tmp_mp[real_name].end()) {
                    tmp_mp[real_name][name] = mp_args[s][i].copy;
				}
			}
			stk.push_back(real_name);
			visit(ptr);
			std::any ans = returning ? std::move(return_value) : std::any(nullptr);
			returning = false;
			return_value = nullptr;
			tmp_mp.erase(real_name);
			stk.pop_back();
			return ans;
		}
		return visit(ctx->atom());
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
                mp_args[name].push_back({node_text(args->tfpdef(i)->NAME()), false, nullptr});
			}
			else {
                mp_args[name].push_back({node_text(args->tfpdef(i)->NAME()), true, visit(args->test(pos++))});
			}
		}
		return nullptr;
    }
	std::any visitFactor(Python3Parser::FactorContext *ctx) override {
		if(ctx->MINUS() == nullptr && ctx->ADD() == nullptr) {
			if(ctx->factor()) return visit(ctx->factor());
			return visit(ctx->atom_expr());
		}
		if(ctx->MINUS() == nullptr) {
            if(ctx->factor()) {
				auto ret = visit(ctx->factor());
				if(std::any_cast <bool> (&ret)) {
					ret = (int2048)(std::any_cast <bool> (ret));
				}
				return ret;
			}
			auto ret = visit(ctx->atom_expr());
			if(std::any_cast <bool> (&ret)) {
				ret = (int2048)(std::any_cast <bool> (ret));
			}
			return ret;
		}
		else {
			std::any tmp;
            if(ctx->factor()) tmp = visit(ctx->factor());
			else tmp = visit(ctx->atom_expr());
			if(std::any_cast <int2048> (&tmp)) {
				return -(std::any_cast <const int2048&> (tmp));
			}
			if(std::any_cast <bool> (&tmp)) {
				return -(int2048)(std::any_cast <bool> (tmp));
			}
			return -(std::any_cast <double> (tmp));
		}
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
				ret_ll = std::any_cast <const int2048&> (tmp);
			}
	    	else {
				ret_db = std::any_cast <double> (tmp);
			}
		}
		for(int i = 1; i < (int)(factors.size()); i++) {
			auto A = visit(factors[i]);
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
                    if(ops[i-1]->STAR()) {
						ret_db *= change_double(std::any_cast <const int2048&> (A));
					}
					else if(ops[i-1]->DIV()) {
						int2048 nw = std::any_cast <const int2048&> (A);
						ret_db /= change_double(nw);
					}
					else if(ops[i-1]->IDIV()) {
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
					if(ops[i-1]->STAR()) {
						ret_ll *= std::any_cast <const int2048&> (A);
					}
					else if(ops[i-1]->DIV()) {
						int2048 nw = std::any_cast <const int2048&> (A);
					    double_flg = true;
						ret_db = change_double(ret_ll) / change_double(nw);
					}
					else if(ops[i-1]->IDIV()) {
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
				if(ops[i-1]->STAR()) {
     				ret_db *= std::any_cast <double> (A);
				}
				else if(ops[i-1]->DIV()) {
					auto nw = std::any_cast <double> (A);
					ret_db /= nw;
				}
				else if(ops[i-1]->IDIV()) {
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
			if(!double_flg) ret_ll = std::any_cast <const int2048&> (tmp);
		    else ret_db = std::any_cast <double> (tmp);
		}
		for(int i = 1; i < (int)(terms.size()); i++) {
			auto A = visit(terms[i]);
			if(std::any_cast <std::string> (&A)) {
                sp_ret += std::any_cast <const std::string&> (A);
				sp_check = true;
				continue;
			}
			if(std::any_cast <bool> (&A)) A = int2048(std::any_cast <bool> (A)); 
			if(std::any_cast <int2048> (&A)) {
                if(double_flg) {
                    if(ops[i-1]->ADD()) {
						ret_db += change_double(std::any_cast <const int2048&> (A));
					}
					else {
						ret_db -= change_double(std::any_cast <const int2048&> (A));
					}
				}
				else {
					if(ops[i-1]->ADD()) {
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
				if(ops[i-1]->ADD()) {
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
	std::any visitExpr_stmt(Python3Parser::Expr_stmtContext *ctx) override {
        auto it = assign_cache.find(ctx);
        if(it == assign_cache.end()) {
            std::vector<std::vector<Python3Parser::TestContext*>> value;
            for(auto ptr : ctx->testlist()) value.push_back(ptr->test());
            it = assign_cache.emplace(ctx,std::move(value)).first;
        }
        const auto &lists = it->second;

	    if(ctx->augassign()) {
			const std::string &s = node_text(ctx->augassign());
			int siz = (int)(lists[0].size());
			if(s == "+=") {
				std::vector <std::any> ret_vec, val_vec;
				for(int B = 0; B < siz; B++) {
					const std::string &A = node_text(lists[0][B]);
                    val_vec.push_back(query_value(A));
				}
			    std::any pre = visit(lists[1][0]);
				if(std::any_cast <std::vector<std::any>> (&pre)) {
					ret_vec = std::any_cast <std::vector<std::any>> (pre);
				}
				else {
					ret_vec.push_back(pre);
					for(int i = 1; i < siz; i++) {
                        ret_vec.push_back(visit(lists[1][i]));
			    	}
				}
                for(int B = 0; B < siz; B++) {
					const auto &ret = ret_vec[B];
			        if(std::any_cast <bool> (&ret)) {
                        int2048 val = (int2048)(std::any_cast <bool> (ret));
                        const std::string &A = node_text(lists[0][B]);
                        const std::any &nw = val_vec[B];
		    			if(std::any_cast <bool> (&nw)) {
		    				val += (int2048)(std::any_cast <bool> (nw));
		    				change_value(A, val);
		      			}
			    		else if(std::any_cast <double> (&nw)) {
		    	   			double vv = change_double(val) + std::any_cast <double> (nw);
			     			change_value(A, vv);
		    			}
			    		else {
                            val += (std::any_cast <const int2048&> (nw));
		    				change_value(A, val);
		    			}
			    	}
					else if(std::any_cast <int2048> (&ret)) {
                    int2048 val = (std::any_cast <const int2048&> (ret));
						const std::string &A = node_text(lists[0][B]);
                        const std::any &nw = val_vec[B];
						if(std::any_cast <bool> (&nw)) {
							val += (int2048)(std::any_cast <bool> (nw));
							change_value(A, val);
						}
						else if(std::any_cast <double> (&nw)) {
							double vv = change_double(val) + std::any_cast <double> (nw);
							change_value(A, vv);
						}
						else {
							val += (std::any_cast <const int2048&> (nw));
							change_value(A, val);
						}
					}
					else if(std::any_cast <double> (&ret)) {
						double val = (std::any_cast <double> (ret));
                        const std::string &A = node_text(lists[0][B]);
                        const std::any &nw = val_vec[B];
						if(std::any_cast <bool> (&nw)) {
							val += (double)(std::any_cast <bool> (nw));
							change_value(A, val);
						}
						else if(std::any_cast <double> (&nw)) {
							double vv = val + std::any_cast <double> (nw);
							change_value(A, vv);
						}
						else {
							val += change_double(std::any_cast <const int2048&> (nw));
							change_value(A, val);
						}
					}
					else {
                        const std::string &A = node_text(lists[0][B]);
						std::string nw = std::any_cast <std::string> (val_vec[B]);
						nw += std::any_cast <const std::string&> (ret);
						change_value(A, nw);
					}
				}
			}
			else if(s == "-=") {
				std::vector <std::any> ret_vec, val_vec;
				for(int B = 0; B < siz; B++) {
					const std::string &A = node_text(lists[0][B]);
                    val_vec.push_back(query_value(A));
				}
			    std::any pre = visit(lists[1][0]);
				if(std::any_cast <std::vector<std::any>> (&pre)) {
					ret_vec = std::any_cast <std::vector<std::any>> (pre);
				}
				else {
					ret_vec.push_back(pre);
					for(int i = 1; i < siz; i++) {
                        ret_vec.push_back(visit(lists[1][i]));
			    	}
				}
				for(int B = 0; B < siz; B++) {
					const auto &ret = ret_vec[B];
				    if(std::any_cast <bool> (&ret)) {
	                    int2048 val = (int2048)(std::any_cast <bool> (ret));
						const std::string &A = node_text(lists[0][B]);
	                    const std::any &nw = val_vec[B];
						if(std::any_cast <bool> (&nw)) {
							val = - val;
							val += (int2048)(std::any_cast <bool> (nw));
							change_value(A, val);
						}
						else if(std::any_cast <double> (&nw)) {
							double vv = - change_double(val) + std::any_cast <double> (nw);
							change_value(A, vv);
						}
						else {
							val = - val;
							val += (std::any_cast <const int2048&> (nw));
							change_value(A, val);
						}
					}
					else if(std::any_cast <int2048> (&ret)) {
	                    int2048 val = (std::any_cast <const int2048&> (ret));
						const std::string &A = node_text(lists[0][B]);
	                    const std::any &nw = val_vec[B];
						if(std::any_cast <bool> (&nw)) {
							val = - val;
							val += (int2048)(std::any_cast <bool> (nw));
							change_value(A, val);
						}
						else if(std::any_cast <double> (&nw)) {
							double vv = - change_double(val) + std::any_cast <double> (nw);
							change_value(A, vv);
						}
						else {
							val = - val;
							val += (std::any_cast <const int2048&> (nw));
							change_value(A, val);
						}
					}
					else if(std::any_cast <double> (&ret)) {
						double val = (std::any_cast <double> (ret));
	                    const std::string &A = node_text(lists[0][B]);
	                    const std::any &nw = val_vec[B];
						if(std::any_cast <bool> (&nw)) {
							val = - val;
							val += (double)(std::any_cast <bool> (nw));
							change_value(A, val);
						}
						else if(std::any_cast <double> (&nw)) {
							double vv = - val + std::any_cast <double> (nw);
							change_value(A, vv);
						}
						else {
							val = - val;
							val += change_double(std::any_cast <const int2048&> (nw));
							change_value(A, val);
						}
					}
				}
			}
			else if(s == "*=") {
				std::vector <std::any> ret_vec, val_vec;
				for(int B = 0; B < siz; B++) {
					const std::string &A = node_text(lists[0][B]);
                    val_vec.push_back(query_value(A));
				}
			    std::any pre = visit(lists[1][0]);
				if(std::any_cast <std::vector<std::any>> (&pre)) {
					ret_vec = std::any_cast <std::vector<std::any>> (pre);
				}
				else {
					ret_vec.push_back(pre);
					for(int i = 1; i < siz; i++) {
                        ret_vec.push_back(visit(lists[1][i]));
			    	}
				}
				for(int B = 0; B < siz; B++) {
					const auto &ret = ret_vec[B];
				    if(std::any_cast <bool> (&ret)) {
	                    int2048 val = (int2048)(std::any_cast <bool> (ret));
						const std::string &A = node_text(lists[0][B]);
	                    const std::any &nw = val_vec[B];
						if(std::any_cast <bool> (&nw)) {
							val *= (int2048)(std::any_cast <bool> (nw));
							change_value(A, val);
						}
						else if(std::any_cast <double> (&nw)) {
							double vv = change_double(val) * std::any_cast <double> (nw);
							change_value(A, vv);
						}
						else if(std::any_cast <int2048> (&nw)) {
							val *= (std::any_cast <const int2048&> (nw));
							change_value(A, val);
						}
						else {
	                        std::string base = std::any_cast <const std::string&> (nw);
							std::string ans = "";
							if(base != "") {
								for(int i = 1; i <= change_ll(val); i++) ans += base;
							}
							change_value(A, ans);
						}
					}
					else if(std::any_cast <int2048> (&ret)) {
	                    int2048 val = (std::any_cast <const int2048&> (ret));
						const std::string &A = node_text(lists[0][B]);
	                    const std::any &nw = val_vec[B];
						if(std::any_cast <bool> (&nw)) {
							val *= (int2048)(std::any_cast <bool> (nw));
							change_value(A, val);
						}
						else if(std::any_cast <double> (&nw)) {
							double vv = change_double(val) * std::any_cast <double> (nw);
							change_value(A, vv);
						}
						else if(std::any_cast <int2048> (&nw)) {
							val *= (std::any_cast <const int2048&> (nw));
							change_value(A, val);
						}
						else {
							std::string base = std::any_cast <const std::string&> (nw);
							std::string ans = "";
							if(base != "") {
								for(int i = 1; i <= change_ll(val); i++) ans += base;
							}
							change_value(A, ans);
						}
					}
					else if(std::any_cast <double> (&ret)) {
						double val = (std::any_cast <double> (ret));
	                    const std::string &A = node_text(lists[0][B]);
	                    const std::any &nw = val_vec[B];
						if(std::any_cast <bool> (&nw)) {
							val *= (double)(std::any_cast <bool> (nw));
							change_value(A, val);
						}
						else if(std::any_cast <double> (&nw)) {
							double vv = val * std::any_cast <double> (nw);
							change_value(A, vv);
						}
						else {
							val *= change_double(std::any_cast <const int2048&> (nw));
							change_value(A, val);
						}
					}
					else {
	                    const std::string &A = node_text(lists[0][B]);
						const std::any &nw = val_vec[B];
	                    int2048 val;
						if(std::any_cast <bool> (&nw)) val = (int2048)(std::any_cast <bool> (nw));
						else val = std::any_cast <const int2048&> (nw);
						std::string base = std::any_cast <const std::string&> (ret);
						std::string ans = "";
						if(base != "") {
							for(int i = 1; i <= change_ll(val); i++) ans += base;
						}
						change_value(A, ans);
					}
				}
			}
			else if(s == "/=") {
				std::vector <std::any> ret_vec, val_vec;
				for(int B = 0; B < siz; B++) {
					const std::string &A = node_text(lists[0][B]);
                    val_vec.push_back(query_value(A));
				}
			    std::any pre = visit(lists[1][0]);
				if(std::any_cast <std::vector<std::any>> (&pre)) {
					ret_vec = std::any_cast <std::vector<std::any>> (pre);
				}
				else {
					ret_vec.push_back(pre);
					for(int i = 1; i < siz; i++) {
                        ret_vec.push_back(visit(lists[1][i]));
			    	}
				}
				for(int B = 0; B < siz; B++) {
					const auto &ret = ret_vec[B];
				    if(std::any_cast <bool> (&ret)) {
	                    int2048 val = (int2048)(std::any_cast <bool> (ret));
						const std::string &A = node_text(lists[0][B]);
	                    const std::any &nw = val_vec[B];
						if(std::any_cast <bool> (&nw)) {
							auto other = (int2048)(std::any_cast <bool> (nw));
							change_value(A, change_double(other) / change_double(val));
						}
						else if(std::any_cast <double> (&nw)) {
							double vv = std::any_cast <double> (nw) / change_double(val);
							change_value(A, vv);
						}
						else {
							auto other = (std::any_cast <const int2048&> (nw));
							change_value(A, change_double(other) / change_double(val));
						}
					}
					else if(std::any_cast <int2048> (&ret)) {
	                    int2048 val = (std::any_cast <const int2048&> (ret));
						const std::string &A = node_text(lists[0][B]);
	                    const std::any &nw = val_vec[B];
						if(std::any_cast <bool> (&nw)) {
							auto other = (int2048)(std::any_cast <bool> (nw));
							change_value(A, change_double(other) / change_double(val));
						}
						else if(std::any_cast <double> (&nw)) {
							double vv = std::any_cast <double> (nw) / change_double(val);
							change_value(A, vv);
						}
						else {
							auto other = (std::any_cast <const int2048&> (nw));
							change_value(A, change_double(other) / change_double(val));
						}
					}
					else if(std::any_cast <double> (&ret)) {
						double val = (std::any_cast <double> (ret));
	                    const std::string &A = node_text(lists[0][B]);
	                    const std::any &nw = val_vec[B];
						if(std::any_cast <bool> (&nw)) {
							double other = (double)(std::any_cast <bool> (nw));
							change_value(A, other / val);
						}
						else if(std::any_cast <double> (&nw)) {
							double vv = std::any_cast <double> (nw) / val;
							change_value(A, vv);
						}
						else {
							double other = change_double(std::any_cast <const int2048&> (nw));
							change_value(A, other / val);
						}
					}
				}
			}
			else if(s == "//=") {
				std::vector <std::any> ret_vec, val_vec;
				for(int B = 0; B < siz; B++) {
					const std::string &A = node_text(lists[0][B]);
                    val_vec.push_back(query_value(A));
				}
			    std::any pre = visit(lists[1][0]);
				if(std::any_cast <std::vector<std::any>> (&pre)) {
					ret_vec = std::any_cast <std::vector<std::any>> (pre);
				}
				else {
					ret_vec.push_back(pre);
					for(int i = 1; i < siz; i++) {
                        ret_vec.push_back(visit(lists[1][i]));
			    	}
				}
				for(int B = 0; B < siz; B++) {
					const auto &ret = ret_vec[B];
				    if(std::any_cast <bool> (&ret)) {
	                    int2048 val = (int2048)(std::any_cast <bool> (ret));
						const std::string &A = node_text(lists[0][B]);
	                    const std::any &nw = val_vec[B];
						if(std::any_cast <bool> (&nw)) {
							auto other = (int2048)(std::any_cast <bool> (nw));
							change_value(A, other / val);
						}
						else if(std::any_cast <double> (&nw)) {
							double vv = std::any_cast <double> (nw) / change_double(val);
							change_value(A, floor(vv));
						}
						else {
							auto other = (std::any_cast <const int2048&> (nw));
						    change_value(A, other / val);
						}
					}
					else if(std::any_cast <int2048> (&ret)) {
	                    int2048 val = (std::any_cast <const int2048&> (ret));
						const std::string &A = node_text(lists[0][B]);
	                    const std::any &nw = val_vec[B];
						if(std::any_cast <bool> (&nw)) {
							auto other = (int2048)(std::any_cast <bool> (nw));
							change_value(A, other / val);
						}
						else if(std::any_cast <double> (&nw)) {
							double vv = std::any_cast <double> (nw) / change_double(val);
							change_value(A, floor(vv));
						}
						else {
							auto other = (std::any_cast <const int2048&> (nw));
							change_value(A, other / val);
						}
					}
					else if(std::any_cast <double> (&ret)) {
						double val = (std::any_cast <double> (ret));
	                    const std::string &A = node_text(lists[0][B]);
	                    const std::any &nw = val_vec[B];
						if(std::any_cast <bool> (&nw)) {
							double other = (double)(std::any_cast <bool> (nw));
							change_value(A, floor(other / val));
						}
						else if(std::any_cast <double> (&nw)) {
							double vv = std::any_cast <double> (nw) / val;
							change_value(A, floor(vv));
						}
						else {
							double other = change_double(std::any_cast <const int2048&> (nw));
							change_value(A, floor(other / val));
						}
					}
				}
			}
			else {
				std::vector <std::any> ret_vec, val_vec;
				for(int B = 0; B < siz; B++) {
					const std::string &A = node_text(lists[0][B]);
                    val_vec.push_back(query_value(A));
				}
			    std::any pre = visit(lists[1][0]);
				if(std::any_cast <std::vector<std::any>> (&pre)) {
					ret_vec = std::any_cast <std::vector<std::any>> (pre);
				}
				else {
					ret_vec.push_back(pre);
					for(int i = 1; i < siz; i++) {
                        ret_vec.push_back(visit(lists[1][i]));
			    	}
				}
				for(int B = 0; B < siz; B++) {
					const auto &ret = ret_vec[B];
				    if(std::any_cast <bool> (&ret)) {
	                    int2048 val = (int2048)(std::any_cast <bool> (ret));
						const std::string &A = node_text(lists[0][B]);
	                    const std::any &nw = val_vec[B];
						if(std::any_cast <bool> (&nw)) {
							auto other = (int2048)(std::any_cast <bool> (nw));
							change_value(A, other % val);
						}
						else if(std::any_cast <double> (&nw)) {
							double other = std::any_cast <double> (nw), k = change_double(val);
							double vv = other / k;
							change_value(A, other - floor(vv) * k);
						}
						else {
							auto other = (std::any_cast <const int2048&> (nw));
							change_value(A, other % val);
						}
					}
					else if(std::any_cast <int2048> (&ret)) {
	                    int2048 val = (std::any_cast <const int2048&> (ret));
						const std::string &A = node_text(lists[0][B]);
	                    const std::any &nw = val_vec[B];
						if(std::any_cast <bool> (&nw)) {
							auto other = (int2048)(std::any_cast <bool> (nw));
							change_value(A, other % val);
						}
						else if(std::any_cast <double> (&nw)) {
							double other = std::any_cast <double> (nw), k = change_double(val);
							double vv = other / k;
							change_value(A, other - floor(vv) * k);
						}
						else {
							auto other = (std::any_cast <const int2048&> (nw));
							change_value(A, other % val);
						}
					}
					else if(std::any_cast <double> (&ret)) {
						double val = (std::any_cast <double> (ret));
	                    const std::string &A = node_text(lists[0][B]);
	                    const std::any &nw = val_vec[B];
						if(std::any_cast <bool> (&nw)) {
							double other = (double)(std::any_cast <bool> (nw));
							change_value(A, other - val * floor(other / val));
						}
						else if(std::any_cast <double> (&nw)) {
							double other = std::any_cast <double> (nw), k = val;
							double vv = other / k;
							change_value(A, other - k * floor(vv));
						}
						else {
							double other = change_double(std::any_cast <const int2048&> (nw));
							change_value(A, other - val * floor(other / val));
						}
					}
				}
			}
		    return nullptr;
		}
		int len = (lists).size(), siz = (lists[0]).size();
		std::vector <std::any> ret_vec{};
		std::any pre = visit(lists[len-1][0]);
		if(std::any_cast <std::vector<std::any>> (&pre)) {
			ret_vec = std::any_cast <std::vector<std::any>> (pre);
		}
		else {
			ret_vec.push_back(pre);
			for(int i = 1; i < siz; i++) {
				ret_vec.push_back(visit(lists[len-1][i]));
			}
		}
		for(int i = 0; i <= len - 2; i++) {
            for(int j = 0; j < siz; j++) {
				const std::string &A = node_text(lists[i][j]);
				change_value(A, ret_vec[j]);
			}
		}
		return nullptr;
	}
    std::any visitComparison(Python3Parser::ComparisonContext *ctx) override {
		auto it = compare_cache.find(ctx);
		if(it == compare_cache.end()) it = compare_cache.emplace(ctx,std::make_pair(ctx->arith_expr(),ctx->comp_op())).first;
		const auto &exprs = it->second.first;
		const auto &ops = it->second.second;
		if(exprs.size()==1) {
            return visit(exprs[0]);
		}
		bool Suc = true;
		int siz = ops.size();
		std::any lst = nullptr;
		for(int i = 0; i < siz; i++) {
			if(!Suc) {
				break;
			}
			const std::string &s = node_text(ops[i]);
			std::any lhs;
			if(i) {
				lhs = std::move(lst);
			}
			else {
				lhs = visit(exprs[i]);
			}
			auto rhs = visit(exprs[i+1]);
			lst = rhs;
			if(s == "!=" || s == "==") {
				if(isNone(lhs)^isNone(rhs)) {
					return (bool)(false);
				}
				if(isString(lhs)^isString(rhs)) {
					return (bool)(false);
				}
                if(isNone(lhs)) {
					Suc &= (s == "==" ? (bool)(true) : (bool) (false));
					continue;
				}
				if(isString(lhs)) {
					bool flg = (std::any_cast <const std::string&> (lhs) == std::any_cast <const std::string&> (rhs));
					Suc &= (s == "==" ? flg : !flg);
					continue;
				}
				bool double_flg = false;
				if(isDouble(lhs)||isDouble(rhs)) {
					double_flg = true;
				}
				if(double_flg) {
					if(s == "==") {
						Suc &= (toDouble(lhs) == toDouble(rhs));
					}
					else {
						Suc &= ((toDouble(lhs)) != toDouble(rhs));
					}
				}
				else {
                    if(s == "==") {
						Suc &= (integer_value(lhs) == integer_value(rhs));
					}
					else {
						Suc &= (integer_value(lhs) != integer_value(rhs));
					}
				}
			}
			else if(s == "<=") {
				if(isString(lhs)||isString(rhs)) {
					Suc &= (std::any_cast <const std::string&> (lhs) <= std::any_cast <const std::string&> (rhs));
				}
				else {
					if(isDouble(lhs)||isDouble(rhs)) {
						Suc &= (toDouble(lhs) <= toDouble(rhs));
					}
					else {
						Suc &= (integer_value(lhs) <= integer_value(rhs));
					}
				}
			}
			else if(s == ">=") {
				if(isString(lhs)||isString(rhs)) {
					Suc &= (std::any_cast <const std::string&> (lhs) >= std::any_cast <const std::string&> (rhs));
				}
				else {
					if(isDouble(lhs)||isDouble(rhs)) {
						Suc &= (toDouble(lhs) >= toDouble(rhs));
					}
					else {
						Suc &= (integer_value(lhs) >= integer_value(rhs));
					}
				}
			}
			else if(s == "<") {
				if(isString(lhs)||isString(rhs)) {
					Suc &= (std::any_cast <const std::string&> (lhs) < std::any_cast <const std::string&> (rhs));
				}
				else {
					if(isDouble(lhs)||isDouble(rhs)) {
						Suc &= (toDouble(lhs) < toDouble(rhs));
					}
					else {
						Suc &= (integer_value(lhs) < integer_value(rhs));
					}
				}
			}
			else if(s == ">") {
				if(isString(lhs)||isString(rhs)) {
					Suc &= (std::any_cast <const std::string&> (lhs) > std::any_cast <const std::string&> (rhs));
				}
				else {
					if(isDouble(lhs)||isDouble(rhs)) {
						Suc &= (toDouble(lhs) > toDouble(rhs));
					}
					else {
						Suc &= (integer_value(lhs) > integer_value(rhs));
					}
				}
			}
		}
		return Suc;
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
        int siz = (ctx->test()).size();
		for(int i = 0; i < siz; i++) {
			bool ret = toBool(visit(ctx->test(i)));
			if(ret == true) {
                visit(ctx->suite(i));
				return nullptr;
			}
		}
		if((ctx->suite()).size() == siz) {
			return nullptr;
		}
		visit(ctx->suite(siz));
		return nullptr;
    }
	std::any visitSuite(Python3Parser::SuiteContext *ctx) override {
		if(ctx->simple_stmt()) return visit(ctx->simple_stmt());
		auto it = suite_cache.find(ctx);
		if(it == suite_cache.end()) it = suite_cache.emplace(ctx,ctx->stmt()).first;
		const auto &stmts = it->second;
		for(auto stmt : stmts) {
			visit(stmt);
			if(returning) break;
		}
		return nullptr;
	}
	std::any visitWhile_stmt(Python3Parser::While_stmtContext *ctx) override {
        while(toBool(visit(ctx->test()))) {
            try {
				visit(ctx->suite());
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
        return nullptr;
    }
    std::any visitContinue_stmt(Python3Parser::Continue_stmtContext *ctx) override {
        throw ContinueSignal{};
		return nullptr;
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
