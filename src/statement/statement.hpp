#ifndef OPS_STATEMENT_HPP
#define OPS_STATEMENT_HPP 1


#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "../utils/json"
#include "../utils/systemfunc"

namespace ops::statement{

using json = nlohmann::json;
using ExecResult = ops::utils::ExecResult;

class StatementSection;

class Statement{
    public:
     virtual ~Statement() = default;

     Statement() = default;
     Statement(std::vector<StatementSection>);
    
     void set_sections(std::vector<StatementSection> new_sections);
    
     const std::vector<StatementSection>& get_sections() const;

     void add_section(std::string name, std::string text);
     void add_section(std::string name, std::string text, int order);

     void load_common(const json& j);
     virtual void load_from_json(const json& j);
     virtual void to_json(json& j);

     ExecResult create_statement_folder(const std::string& folder_path = ".");

    protected:
     std::map<std::string, std::string> sections;
};

class StatementSection{
    public:
     virtual ~StatementSection() = default;

     StatementSection() = default;
     StatementSection(std::string name, std::string text);

     void set_name(std::string name);
     void set_text(std::string text);

     std::string& get_name() const;
     std::string& get_text() const;
    
    protected:
     std::string name;
     std::string text;
};



} //namespace ops::statement


#endif //OPS_STATEMENT_HPP