#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <stdint.h>
#include <unistd.h>
#include <getopt.h>
#include <stdexcept>
#include <endian.h>

#include "snappy.h"
#include "snappy-stream.h"

struct options_t
{
    options_t()
        : decompress(false)
        , test(false)
        , stdout(false)
        , verbose(false)
        , chunksize(32768)
    {
    }
    std::vector<std::string> filenames;
    bool decompress;
    bool test;
    bool stdout;
    bool verbose;
    unsigned chunksize;
};

void usage()
{
        std::cerr << "Usage: snap [-d] [-t] [-c] [-s chunksize] [name ...]\n"
                  << "  -d, --decompress  decompress\n"
                  << "  -c, --stdout      write output to stdout\n"
                  << "  -t, --test        test archive for integrity, ignored when -d is given\n"
                  << "  -v, --verbose     Verbose. Display original and compressed file sizes\n"
                  << "  -s, --chunksize   chunk size for compression, ignored when -d is given\n\n"
                  << "  when no filename is given the input is read from stdin, output to stdout\n"
                  << std::endl;
        exit(1);
}

int snap(std::istream *in, std::ostream *out, const options_t *options)
{
    snappy::osnapstream osn(out, options->chunksize);
    osn << in->rdbuf();
    osn.terminate();
    int rc = (!osn || !(*out));
    if (rc)
        std::cerr << "Error compressing\n";
    return rc;
}


int unsnap(std::istream *in, std::ostream *out, const options_t *options)
{
    snappy::isnapstream isn(in);
    isn >> out->rdbuf();
    int rc = (!isn || !(*out));
    if (rc)
        std::cerr << "Error decompressing\n";
    return rc;
}

namespace {
class nullbuf: public std::streambuf {
    int_type overflow(int_type c)
    {
        return traits_type::not_eof(c);
    }
};
}

int test(std::istream *in, const options_t *options)
{
    snappy::isnapverifystreambuf isnverifybuf(in->rdbuf());
    std::istream isn(&isnverifybuf);
    nullbuf nbuf;
    std::ostream devnull(&nbuf);
    int rc = 0;

    isn.exceptions ( std::istream::failbit | std::istream::badbit );
    try {
        isn >> devnull.rdbuf();
    } catch (const std::exception &e) {
        std::cerr << "Test failed: " << e.what() << "\n\n";
        return 1;
    }

    std::cerr << "Test passed\n";
    if (options->verbose) {
        std::cerr << "Number of chunks: " << isnverifybuf.nchunks()
                  << "\nOriginal size: " << isnverifybuf.original_size()
                  << "\nCompressed size: " << isnverifybuf.compressed_size()
                  << "\n\n";
    }

    return 0;
}

bool parse_cmd_options(int argc, char* argv[], options_t* options)
{
    while (1)
    {
        static struct option long_options[] = {
            {"decompress", no_argument, 0, 'd'},
            {"stdout", no_argument, 0, 'c'},
            {"test", no_argument, 0, 't'},
            {"verbose", no_argument, 0, 'v'},
            {"chunksize", required_argument, 0, 's'},
            {0, 0, 0, 0}
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "dctvs:", long_options, &option_index);

        if (c == -1)
            break;

        switch (c)
        {
            case 0:
                break;
            case 'd':
                options->decompress = true;
                break;
            case 'c':
                options->stdout = true;
                break;
            case 't':
                options->test = true;
                break;
            case 'v':
                options->verbose = true;
                break;
            case 's':
                options->chunksize = atoi(optarg);
                break;
            case '?':
                return false;
            default:
                return false;
        }
    }

    if (optind >= argc)
        options->stdout = true;
    else
        while (optind < argc) {
            options->filenames.push_back(argv[optind++]);
        }

    return true;
}

#if 0
void debug_print_options(const options_t* options)
{
    std::cerr << "OPTIONS:"
              << "\n-d = " << options->decompress
              << "\n-c = " << options->stdout
              << "\n-t = " << options->test
              << "\n-s = " << options->chunksize
              << std::endl;
}
#endif

int action(const std::string filename, const options_t* options)
{
    std::ifstream in_file;
    std::ofstream out_file;
    std::istream *in = NULL;
    std::ostream *out = NULL;

    if (!filename.empty())
    {
        in_file.open(filename.c_str(), std::ios::binary);
        if (!in_file.is_open() || !in_file.good())
        {
            std::cerr << "Cant open input file" << std::endl;
            return 1;
        }
        in = &in_file;
    }
    else
        in = &std::cin;

    if (options->stdout || filename.empty())
        out = &std::cout;
    else if (!options->test)
    {
        std::string output_filename;
        if (options->decompress)
        {
            const std::string &infn = filename;

            if (infn.size() > 3 &&
                !infn.compare(infn.size()-3, 3, ".sn"))
                output_filename = infn.substr(0, infn.size()-3);
            else {
                std::cerr << "unknown suffix -- ignored\n";
                return 1;
            }
        }
        else
            output_filename = filename + ".sn";

        out_file.open(output_filename.c_str(),
                      std::ios::out | std::ios::trunc | std::ios::binary);
        if (!out_file.good())
        {
            std::cerr << "Can not open output file" << std::endl;
            return 1;
        }
        out = &out_file;
    }

    // perform compression/decompression
    if (options->decompress)
        return unsnap(in, out, options);
    else if (options->test)
        return test(in, options);
    else
        return snap(in, out, options);

    return 0;
}

int main(int argc, char* argv[])
{
    options_t options;
    if (!parse_cmd_options(argc, argv, &options))
        usage();

    int rc = 0;
    if (!options.filenames.size())
        options.filenames.push_back("");
    std::vector<std::string>::const_iterator it = options.filenames.begin();
    for (; it != options.filenames.end(); ++it)
        rc |= action(*it, &options);

    return rc;
}
