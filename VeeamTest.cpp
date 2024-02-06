#define _CRT_SECURE_NO_WARNINGS // This line is added to prevent an error Visual Studio throw when ctime library is used 

#include <filesystem> 
#include <fstream> 
#include <sstream>
#include <iostream>
#include <ctime>
#include <chrono>
#include <thread>


using namespace std;
using namespace std::filesystem;

/*
log function gets path of source and replica folders, name of the file and the action
print it to console and write it into log.txt file
stringstream used for simplicity of the code and converted to string for print and write operations
*/
void log(path src, path rep, path file, string log_message)
{
	ofstream log_file;
	ostringstream log_stream;
	time_t currentime = chrono::system_clock::to_time_t(chrono::system_clock::now());
	
	log_stream << string(ctime(&currentime)) << "::Source: " << src <<" -> Replica: " << rep << "  --  File: " << file << " " << log_message << "\n::\n" << endl;

	cout << log_stream.str();

	log_file.open("logs.txt", std::ios_base::app);
	log_file << log_stream.str();
	log_file.close();
}

/*
synch_folder function synchronizes the folders in subject
filesystem library is playing a huge role for this operation
first by using iterator in "for" loop each file of source foulder is created in replica folder depending on existence or copied to replica folder depending on time stamp of last change
second by using iterator in "for" loop each file of replica foulder is checked wether it exist in source folder or not, non-existing files are deleted
*/
void synch_folder(path src, path rep)
{
	for (const auto& entry : directory_iterator(src))
	{
		path source_file = entry.path();
		path replica_File = rep / source_file.filename();

		if (exists(replica_File)&&(last_write_time(replica_File) != last_write_time(source_file)))
		{
			copy(source_file, replica_File, copy_options::overwrite_existing);
			log(src, rep, source_file.filename(), "COPIED");
		}
		else if (!exists(replica_File))
		{
			copy(source_file, replica_File);
			log(src, rep, source_file.filename(), "CREATED");
		}
	}

	for (const auto& entry : directory_iterator(rep))
	{
		path replica_File = entry.path();
		path source_file = src / replica_File.filename();

		if (!exists(source_file))
		{
			remove(replica_File);
			log(src, rep, source_file.filename(), "REMOVED");
		}
	}
		

}

/*
main file is only responsible of taking the paths of source and replice folders as command line arguments and periodically call the synchronization function
currently synchronization function is called once per ten seconds
*/
int main(int argc, char* argv[])
{
	path src = argv[argc - 2];
    path rep = argv[argc - 1];

	while (true)
	{
		synch_folder(src, rep);
		this_thread::sleep_for(chrono::seconds(10));
	}

	return 0;
}

