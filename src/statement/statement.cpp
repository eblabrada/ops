#include "statement.hpp"
#include <fstream>
#include <iostream>

namespace stmt = ops::statement;

stmt::Statement::Statement(std::string code, std::vector<StatementSection> sections) : code(code), sections(sections) {}

void stmt::Statement::init(){
    sections = {
        StatementSection("legend"),
        StatementSection("input"),
        StatementSection("output"),
        StatementSection("examples"),
        StatementSection("notes")
    };    
}

void stmt::Statement::set_code(std::string new_code) { code = new_code; }
void stmt::Statement::set_sections(std::vector<StatementSection> new_sections) { sections = new_sections; }

const std::string& stmt::Statement::get_code() const{ return code; }
const std::vector<stmt::StatementSection>& stmt::Statement::get_sections() const{ return sections; }

void stmt::Statement::add_section(std::string name, std::string text){
    sections.push_back(stmt::StatementSection(name, text));
}

void stmt::Statement::add_section(std::string name, std::string text, int order){
    order--;
    sections.insert(sections.begin() + order, stmt::StatementSection(name, text));
}

void stmt::Statement::load_common(const json& j){
    try{
        if(!j.contains("sections") || !j["sections"].is_array()){
            return;
        }

        for(const auto& section : j["sections"]){
            std::string name = section.value("name", "section " + std::to_string(section.size()));
            std::string text = section.value("text", "");
        }
    }
    catch(const json::type_error& e){
        std::cerr<<e.what();
    }
}

void stmt::Statement::load_from_json(const json& j){ load_common(j); }

void stmt::Statement::to_json(json& j) const{
    std::vector<json> json_sections;

    for(const auto& section : sections){
        json j_obj;
        section.to_json(j_obj);
        json_sections.push_back(j_obj);
    }

    j = json{
        {"sections", json_sections}
    };
}

stmt::ExecResult stmt::Statement::create_statement_folder(const std::string& folder_path = "."){
    if(!this->statement_folder.empty()){
        return ExecResult{1, "statement folder already exists"};
    }

    this->statement_folder = folder_path + "/" + this->code;

    if(!utils::create_directory(statement_folder).success()) {
      return ExecResult{2, "failed to create task folder"};
    }

    json j;
    this->to_json(j);
    std::string json_file_path = statement_folder + "/statement.json";

    std::ofstream fout(json_file_path);

    if (!fout.is_open()) {
      throw std::runtime_error("Failed to open task JSON file for writing: " +
                               json_file_path);
    }

    fout << j.dump(4) << std::endl;

    fout.close();

    if(!utils::create_directory(statement_folder+"/sections").success()) {
      return ExecResult{2, "failed to create sections folder"};
    }

    for(const auto& section : sections){
        std::string section_path = statement_folder + "/" + section.get_name();
        fout.open(section_path);
        if (!fout.is_open()) {
            throw std::runtime_error("Failed to open section file for writing: " +
                               json_file_path);
        }
        fout << section.get_text();
        fout.close();
    }

    if(!utils::create_directory(statement_folder+"/images").success()) {
      return ExecResult{2, "failed to create images folder"};
    }
    

    return ExecResult{0, "task folder created successfully"};
}

stmt::StatementSection::StatementSection(std::string name, std::string text) : name(name), text(text) {}

void stmt::StatementSection::set_name(std::string new_name) { name = new_name; }
void stmt::StatementSection::set_text(std::string new_text) { text = new_text; }

const std::string& stmt::StatementSection::get_name() const{ return name; }
const std::string& stmt::StatementSection::get_text() const{ return text; }

void stmt::StatementSection::to_json(json& j) const{
    j = json{
        {"name", name},
        {"text", text}
    };
}