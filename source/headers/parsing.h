#ifndef USERINPUT_PARSING_HEADER
#define USERINPUT_PARSING_HEADER

#include <fstream>
#include "structures.h"

// determines whether the program should run in portable mode or not, see definition for more info
bool portable_condition();

// returns an object item to its initial state
void reset_object(mineral_entry & data);
void reset_object(collection_entry & data);

// converts an int or bool to a string form
std::string byte_to_string(int);
std::string byte_to_string(bool);

// returns the part of a file path after the last slash, i.e. the file name
std::string namepart(std::string filepath);

// returns 's' if the argument is not 1
inline const char * plural_correction(const int);

// gets the filesize of a file
std::ifstream::streampos filesize(const char* filename);

// converts a string to a boolean value
bool to_bool(const std::string s);

// removes all non-alphabetic characters from a string
int censor(std::string &word);

// limits a string to 32 characters, and adds ellipsis if string is shortened
std::string prune(std::string s);

// checks if a file exists
bool does_file_exist(const std::string name);

// separates a string on a given character
std::vector<std::string> explode(std::string s, char c);

// checks if a string is a valid integer
bool is_integer(const std::string s, bool allow_void = true);

// checks if a string contains no numbers
bool is_antinumber(const std::string s);

// checks if a string is either 1 or 0
bool is_binary(const std::string s);

// converts a vector of strings to a string of comma separated values
std::string vec_to_str(std::vector<std::string> v, bool returntilde = false);
// converts a vector of strings to a string of file seperator (ASCII 28, 0x1c) separated values
std::string vec_to_sepstring(std::vector<std::string> v, bool returntilde = false);

// converts the string "\\n" to the character '\n' in the given string
void escape_string(std::string &s);

// converts date of form dd-mm-yyyy (what the database stores dates as) to DD Month yyyy
std::string shrt_long_date(std::string shortdate);

// converts an amount in pence to properly formatted money - £x.yz is pence > 99, otherwise Xp
std::string pence_to_pounds(int pence);

//                |  texture=0  |  texture=1
//  ------------------------------------------
//  capital=true  |    Rough    |   Tumbled
//  capital=false |    rough    |   tumbled
std::string rough_or_tumbled(int texture, bool capital = false);

// prints out a vector of strings as a list
std::string visualise_vector(std::vector<std::string> v);

// replaces all forward slashes in a string with backslashes
void sanitise_path(std::string &path);

// creates an example file to be read by the add_from_file function
void example_file(bool collection);

// verify a file to be read by add_from_file(), returning a list of syntactical errors
std::vector<std::string> parse_input_file(std::string filename, std::string collection, int errorfree = 0);

// like vec_to_str but for ints rather than strings
std::string vector_int_to_string(std::vector<int> v);

// the inverse of vector_int_to_string
std::vector<int> string_to_vector_int(std::string s);

#endif
