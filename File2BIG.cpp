// This program splits and recombines files to a maximum of one gigabyte in length
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

void print_usage(const char * const program_name)
{
  std::cout <<
    "usage: " << program_name << " mode prefix input" << std::endl <<
    "  mode: the action can be \"split\" or \"join\"" << std::endl <<
    "  prefix: the prefix of the resulting file(s)" << std::endl <<
    "  input: the file(s) either split or combined" << std::endl;
}

void pre_exit(const char * const exit_message)
{
  // Print the exit message
  std::cout << std::endl << exit_message << std::endl;

  // Wait for user input to confirm execution
  std::cout << "Press Enter to continue";
  getchar();
}
#define confirm(exit_message, exit_value) pre_exit(exit_message); return exit_value

class CharBuffer
{
public:
  // Base constructor/destructor
  CharBuffer(int Size) { size = Size; buffer = new char[size]; }
  ~CharBuffer() { delete[] buffer; }

  // Get the size of the buffer in characters
  int Size() { return size; }
  // Get the location of the buffer
  char * Buffer() { return buffer; }

private:
  // Disallow construction copying
  CharBuffer(const CharBuffer & other) {}
  // Disallow assignment copying
  CharBuffer& operator=(const CharBuffer & other) {}

  // The size of the allocated buffer
  int size;
  // The buffer allocated
  char * buffer;
};

int main(int argc, char *argv[])
{
  // The minimum number of arguments that must be given on the command line
  // 1. Program name
  // 2. Action to perform
  // 3. Output filename prefix
  // 4. At least one input filename
  const int min_arg_count = 4;
  const int byte = 1;
  const int kilobyte = byte * 1024;
  const int megabyte = kilobyte * 1024;
  const int gigabyte = megabyte * 1024;
  const int split_file_size = gigabyte;

  // Double check that we have enough arguments
  if (argc < min_arg_count)
  {
    print_usage(argv[0]);
    confirm("not enough arguments", 1);
  }

  // Check which action we are performing
  bool split = strcmp(argv[1], "split") == 0;
  bool join = strcmp(argv[1], "join") == 0;
  if (split == join)
  {
    print_usage(argv[0]);
    confirm("mode is invalid", 1);
  }

  // Get the file name(s) to operate on
  std::vector<std::string> in_filenames;
  for (int i = 3; i < argc; ++i)
  {
    in_filenames.push_back(argv[i]);
  }
  const int in_file_count = static_cast<int>(in_filenames.size());
  if (join && (in_file_count < 2))
  {
    print_usage(argv[0]);
    confirm("join requires at least two input files", 1);
  }

  // Double check that we can open the source file(s)
  std::vector<std::ifstream> in_files;
  for (int i = 0; i < in_file_count; ++i)
  {
    // Attempt to open the file
    const std::string & s = in_filenames[i];
    in_files.push_back(std::ifstream(s, std::ios::binary));

    // Check that the file opened
    const std::ifstream & f = in_files[i];
    if (!f.is_open())
    {
      print_usage(argv[0]);
      confirm("source file(s) could not be opened", 1);
    }
  }

  // Create a buffer to read to and write from
  CharBuffer buffer(split_file_size);

  // Perform the action
  if (split)
  {
    // Read the file into buffer segments and write to smaller files
    std::string prefix = argv[2];
    for (int segment = 0; in_files[0]; ++segment)
    {
      // Open the destination file
      std::string out_filename = prefix + std::to_string(segment);
      std::ofstream out_file(out_filename, std::ios::binary | std::ios::trunc);
      if (!out_file.is_open())
      {
        print_usage(argv[0]);
        confirm("destination file(s) could not be opened", 1);
      }

      // Read a segment of the file
      in_files[0].read(buffer.Buffer(), buffer.Size());

      // Write the number of characters that we read
      int bytes_read = static_cast<int>(in_files[0].gcount());
      if (bytes_read > 0)
      {
        out_file.write(buffer.Buffer(), bytes_read);

        std::cout <<
          "Wrote " << bytes_read << " bytes" <<
          " to \"" << out_filename << "\"" << std::endl;
      }
    }
  }
  else
  {
    // Open the destination file
    std::ofstream out_file(argv[2], std::ios::binary | std::ios::trunc);
    if (!out_file.is_open())
    {
      print_usage(argv[0]);
      confirm("destination file could not be opened", 1);
    }
    
    // Write the contents of the source files
    for (int i = 0; i < in_file_count; ++i)
    {
      const std::string & s = in_filenames[i];
      std::ifstream & f = in_files[i];
      while (f)
      {
        // Read a segment of the file
        f.read(buffer.Buffer(), buffer.Size());

        // Write the number of characters that we read
        int bytes_read = static_cast<int>(f.gcount());
        if (bytes_read > 0)
        {
          out_file.write(buffer.Buffer(), bytes_read);

          std::cout <<
            "Wrote " << bytes_read << " bytes" <<
            " from \"" << s << "\"" << std::endl;
        }
      }
    }
  }

  confirm("Success", 0);
}
