#include <io.h>
#include <fstream>
#include <iostream>
#include <cstdint>
#include <filesystem>

#include <napi.h>

#include "src/algo.h"

/// The number of arguments expected by the matching algorithm
#define MATCH_ALGO_ARG_COUNT 5


Napi::Value Match(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    //check argument count
    if (info.Length() != MATCH_ALGO_ARG_COUNT) {
        Napi::Error::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    //check argument values
    for (int i = 0; i < MATCH_ALGO_ARG_COUNT; i++) {
        //check argument i
        auto arg = info[i];

        //make sure it is a string
        if (!arg.IsString()) {
            Napi::TypeError::New(env, "MATCHING-ALGORITHM: Arguments should be strings!").ThrowAsJavaScriptException();
            return env.Null();
        }
    }

    //for the first 3 arguments, check whether this file exists or not
    for (int i = 0; i < MATCH_ALGO_ARG_COUNT - 2; i++) {
        //check if file exist
        std::string fileName = info[i].As<Napi::String>();

        if(!std::filesystem::exists(fileName)) {
            Napi::Error::New(env, "MATCHING-ALGORITHM: Input file " + fileName + " does not exist").ThrowAsJavaScriptException();
            return env.Null();
        }
    }

    //for the last 2 arguments, make sure that the parent folder exists and the path is really a filename path (not a path to a directory)
    for (int i = MATCH_ALGO_ARG_COUNT - 2; i < MATCH_ALGO_ARG_COUNT; i++) {
        std::string outputFileName = info[i].As<Napi::String>();
        std::filesystem::path outputPath = std::filesystem::path(outputFileName);
        std::filesystem::path parentPath = outputPath.parent_path();
        if (!std::filesystem::exists(parentPath)) {
            Napi::Error::New(env, "MATCHING-ALGORITHM: Output folder " + outputFileName + " does not exist. Make sure, it exists, otherwise the algorithm will not work.").ThrowAsJavaScriptException();
            return env.Null();
        }
        if (!outputPath.has_filename()) {
            Napi::Error::New(env, "MATCHING-ALGORITHM: Output file " + outputFileName + " is not a path to a file. Make sure that the output parameter really describes a path to a file.").ThrowAsJavaScriptException();
            return env.Null();
        }
    }

    //get the arguments...
    std::string arg0 = info[0].As<Napi::String>();
    std::string arg1 = info[1].As<Napi::String>();
    std::string arg2 = info[2].As<Napi::String>();
    std::string arg3 = info[3].As<Napi::String>();
    std::string arg4 = info[4].As<Napi::String>();

    //perform the algorithm
    CS::perform_algo_with_file_names(arg0.c_str(), arg1.c_str(), arg2.c_str(), arg3.c_str(), arg4.c_str());
    return env.Null();
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "matching"), Napi::Function::New(env, Match));
    return exports;
}

NODE_API_MODULE(matching, Init)

