//CS300 Project Two
//Benjamin Holcomb
//4-17-2026

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using std::cin;
using std::cout;
using std::endl;
using std::getline;
using std::string;
using std::stringstream;
using std::vector;

//Structure to hold course data
struct Course {
	string courseNumber;
	string courseTitle;
	vector<string> prerequisites;
};

//Global vector holding courses
vector<Course> vectorCourseList;

//helper to trim whitespace
static inline string trim(const string &s) {
	size_t start = 0;
	while (start < s.size() && std::isspace((unsigned char)s[start])) ++start;
	if (start == s.size()) return "";
	size_t end = s.size() - 1;
	while (end > start && std::isspace((unsigned char)s[end])) --end;
	return s.substr(start, end - start + 1);
}
//helper to ignore case, as in the example input from the assignment(csci400 vs CSCI400, or CsCi400)
static inline string toUpperStr(const string& s) {
	string out = s;
	for (char &c : out) c = (char)std::toupper((unsigned char)c);
	return out;
}

//Loads courses from file into vectorCourseList
void Vector_LoadCourses(const string& fileName) {
	std::ifstream in(fileName);
	if (!in.is_open()) {
		cout << "Error: File could not be accessed." << endl;
		return;
	}

	vectorCourseList.clear(); //clear existing data

	string line;
	int lineNo = 0;
	while (std::getline(in, line)) {
		++lineNo;
		//check for blank line (after trimming)
		string tline = trim(line);
		if (tline.empty()) {
			cout << "Error: Blank line found at at line" << lineNo << "." << endl;
			continue;
		}

		//split by commas
		vector<string> tokens;
		string token;
		stringstream ss(line);
		while (std::getline(ss, token, ',')) {
			tokens.push_back(trim(token));
		}

		//Validate minimum tokens (courseNumber and title)
		if (tokens.size() < 2) {
			cout << "Error: invalid line format (missing course number or title at line " << lineNo << "." << endl;
			continue;
		}
		
		if (tokens[0].empty()) {
			cout << "Course numbre is missing at line " << lineNo << "." << endl;
			continue;
		}
		
		if (tokens[1].empty()) {
			cout << "Course title is missing at line" << lineNo << "." << endl;
			continue;
		}

		Course newCourse;
		newCourse.courseNumber = toUpperStr(tokens[0]);
		newCourse.courseTitle = tokens[1];
		newCourse.prerequisites.clear();

		bool validLine = true;

		//process prerequisite tokens (if there are any).  Skip empty prerequisite tokens with a warning
		for (size_t i = 2; i < tokens.size(); ++i) {
			if (tokens[i].empty()) {
				cout << "Warning: Empty prerequisite token skipped for " << newCourse.courseNumber << " at line " << lineNo << "." << endl;
				continue; //skip empty token
			}
			newCourse.prerequisites.push_back(toUpperStr(tokens[i]));
		}
		//Always add the course even if some prereq tokens are empty
		vectorCourseList.push_back(std::move(newCourse));

	}
	//Cross-reference validation, ensure that every prerequisite 
	// exists somewhere in the vectorCourseList
	for (const auto& course : vectorCourseList) {
		for (const auto& prereqID : course.prerequisites) {
			bool found = false;
			for (const auto& match : vectorCourseList) {
				if (match.courseNumber == prereqID) {
					found = true;
					break;
				}
			}
			if (!found) {
				cout << "Error: Prerequisite " << prereqID << " for " << course.courseNumber << " not found in file." << endl;
			}
		}
	}

	cout << "Data loaded successfully!" << endl;
}

//Print all courses alphanumerically by course number
void Vector_PrintAllSorted() {
	if (vectorCourseList.empty()) {
		cout << "Error: No data loaded. Please load data first (Option 1)." << endl;
		return;
	}

	//Make a copy and sort by courseNumber
	vector<Course> sorted = vectorCourseList;
	std::sort(sorted.begin(), sorted.end(), [](const Course& a, const Course& b) {
		return a.courseNumber < b.courseNumber;
		});

	cout << "Alphanumeric course list: " << endl;
	for (const auto& c : sorted) {
		cout << c.courseNumber << " - " << c.courseTitle << endl;
	}
}

//search for a cousr by course number (includes prerequistes)
void Vector_SearchAndPrint(const string& searchID) {
	string key = toUpperStr(searchID);
	bool found = false;
	for (const auto& course : vectorCourseList) {
		if (course.courseNumber == key) {
			cout << "Course Number: " << course.courseNumber << endl;
			cout << "Course Title: " << course.courseTitle << endl;
			if (course.prerequisites.empty()) {
				cout << "Prerequisites: None" << endl;
			}
			else {
				//for each prerequisite, attempt to find its title for display
				cout << "Prerequisites:";
				
				bool first = true;
				for (const auto &pid : course.prerequisites) {
					//find course with course numer == pid (prerequisite ID)
					string title = "";
					for (const auto &match : vectorCourseList) {
						if (match.courseNumber == pid) {
						title = match.courseTitle;
						break;
					}
				}
				if (!first) cout << ",";
				cout << " " << pid;
				if (!title.empty()) cout << " (" << title << ")";
				first = false;
				}
			cout << endl;
			}
		found = true;
		break;
		}
	}
    if (!found) {
	    cout << "Error: Couurse " << searchID << " not found." << endl;
     }
}

//main function
int main() {
	int userOption = 0;
	cout << "Welcome to ABCU Course Planner!" << endl;

	while (userOption != 9) {
		cout << endl;
		cout << "1. Load Data Structure" << endl;
		cout << "2. Print Alphanumerically Ordered Listof All Courses:" << endl;
		cout << "3. Print Course Title and Prerequisites" << endl;
		cout << "9. Exit" << endl;
		cout << "What would you like to do?" << endl;

		//Read input line and attempt to parse integer option (prevents stream issues)
		string input;
		if (!std::getline(cin, input)) {
			//EOF or error
			break;
		}
		input = trim(input);
		if (input.empty()) {
			cout << "Invalid input, please try again." << endl;
			continue;
		}

		//Try parse integer
		try {
			userOption = std::stoi(input);
		}
		catch (...) {
			cout << "Invalid option, please try again." << endl;
			continue;
		}

		//menu options
		switch (userOption) {
		case 1: {
			//Prompt for file name
			cout << "Enter the course data filename: " << endl;
			string filename;
			if (!std::getline(cin, filename)) {
				cout << "Error reading filename." << endl;
			}
			else {
				filename = trim(filename);
				if (filename.empty()) {
					cout << "Filename cannot be empty!" << endl;
				}
				else {
					Vector_LoadCourses(filename);
				}
			}
			break;

		}
		case 2:
			Vector_PrintAllSorted();
			break;
		case 3: {
			cout << "What course do you want to know about?" << endl;
			string searchID;
			if (!std::getline(cin, searchID)) {
				cout << "Error reading input.";
			}
			else {
				searchID = trim(searchID);
				if (searchID.empty()) {
					cout << "Error: Course ID cannot be empty." << endl;
				}
				else {
					Vector_SearchAndPrint(searchID);
				}
			}
			break;
		}
		case 9:
			cout << "Thank you for using the course planner!" << endl;
			break;
		default:
			cout << "Invalid option, please try again." << endl;
			break;
		}
	}

	return 0;

}

