#ifndef OPS_STATEMENT_HPP
#define OPS_STATEMENT_HPP 1

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
     Statement(std::string code, std::vector<StatementSection> sections);

     void init();

     void set_code(std::string new_code);
     void set_sections(std::vector<StatementSection> new_sections);

     const std::string& get_code() const;
     const std::vector<StatementSection>& get_sections() const;

     void add_section(std::string name, std::string text);
     void add_section(std::string name, std::string text, int order);

     void load_common(const json& j);
     virtual void load_from_json(const json& j);
     virtual void to_json(json& j) const;

     ExecResult create_statement_folder(const std::string& folder_path = ".");

    protected:
     std::string code;
     std::vector<StatementSection> sections;
     std::string statement_folder;
};

class StatementSection{
    public:
     virtual ~StatementSection() = default;

     StatementSection() = default;
     StatementSection(std::string name, std::string text = "");

     void set_name(std::string new_name);
     void set_text(std::string new_text);

     const std::string& get_name() const;
     const std::string& get_text() const;

     virtual void to_json(json& j) const;
    
    protected:
     std::string name;
     std::string text;
};



} //namespace ops::statement


#endif //OPS_STATEMENT_HPP