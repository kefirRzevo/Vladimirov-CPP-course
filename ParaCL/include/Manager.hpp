#pragma once

#include <string>
#include <optional>
#include <filesystem>
#include <boost/program_options.hpp>

#include "frontend/Driver.hpp"
#include "backend/NodeCodegen.hpp"
#include "backend/VirtualMachine.hpp"

namespace po = boost::program_options;

namespace paracl
{

class Manager
{
private:
    std::optional<std::string> inputFile_ = std::nullopt;
    std::optional<std::string> dumpFile_ = std::nullopt;
    std::optional<std::string> decodeFile_ = std::nullopt;
    std::optional<std::string> disasmFile_ = std::nullopt;
    bool help_ = false;

    po::options_description cmdline_{"Options"};
    po::options_description visible_{"Allowed"};
    po::positional_options_description pos_;

    void initProgramOptions() {
        po::options_description generic("Generic options");
        generic.add_options()
            ("help,h", "help message");

        po::options_description config("Configuration");
        config.add_options()
            ("dump", po::value<std::string>()->implicit_value(""),
            "ast dot dump")
            ("decode",
            po::value<std::string>()->implicit_value(""),
            "decode ast")
            ("disasm", po::value<std::string>()->implicit_value(""),
            "my assembler");

        po::options_description hidden("Hidden options");
        hidden.add_options()
            ("input", po::value<std::string>()->required(),
            "input file");

        visible_.add(generic).add(config);
        cmdline_.add(generic).add(config).add(hidden);
        pos_.add("input", 1);
    }

    void parseProgramOptions(int argc, const char* argv[]) {
        po::variables_map vm;
        try {
            auto parser = po::command_line_parser(argc, argv);
            po::store(parser.options(cmdline_).positional(pos_).run(), vm);
            po::notify(vm);

            if (vm.count("help")) {
                std::cout << visible_ << std::endl;
                help_ = true;
                return;
            }
            std::filesystem::path fullpath;
            if (vm.count("input")) {
                auto input = vm["input"].as<std::string>();
                fullpath = input;
                inputFile_.emplace(input);
            }
            fullpath.replace_extension();
            if (vm.count("dump")) {
                auto dump = vm["dump"].as<std::string>();
                if (dump == "") {
                    dump = fullpath.generic_string().append(".dot");
                }
                dumpFile_.emplace(dump);
            }
            if (vm.count("decode")) {
                auto decode = vm["decode"].as<std::string>();
                if (decode == "") {
                    decode = fullpath.generic_string().append(".decoded");
                }
                decodeFile_.emplace(decode);
            }
            if (vm.count("disasm")) {
                auto disasm = vm["disasm"].as<std::string>();
                if (disasm == "") {
                    disasm = fullpath.generic_string().append(".myasm");
                }
                disasmFile_.emplace(disasm);
            }
        } catch (const po::required_option& e) {
            if (vm.count("help")) {
                std::cout << visible_ << std::endl;
                help_ = true;
                return;
            } else {
                throw;
            }
        }
    }

    void process() {
        if (help_) {
            return;
        }
        Driver drv;
        drv.setFilepath(inputFile_.value());
        drv.parse();
        drv.semanticAnalyze();
        if (drv.hasErrors()) {
            drv.reportAllErrors(std::cerr);
            return;
        }
        if (dumpFile_.has_value()) {
            drv.dumpAST(dumpFile_.value());
        }
        if (decodeFile_.has_value()) {
            drv.decodeAST(decodeFile_.value());
        }

        NodeCodegen codegener;
        auto im = codegener.codegen(drv.getRoot());
        if (disasmFile_.has_value()) {
            im.disassembble(disasmFile_.value());
        }
        VirtualMachine m;
        m.loadImage(std::move(im));
        m.execute();
    }

public:
    Manager() = default;

    void run(int argc, const char* argv[]) {
        initProgramOptions();
        parseProgramOptions(argc, argv);
        process();
    }
};

} // namespace paracl
