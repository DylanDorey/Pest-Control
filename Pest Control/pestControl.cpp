/***************************************
Pest Control
Author: Dylan Dorey
Date Completed: 12/19/24
Description: Assigns 1 high impact, 1 medium impact, and 1 low impact bug (oldest first) to a specified amount of developers based upon an XML file containing a database of reported bugs
***************************************/

#include <iostream>
#include <fstream>
#include <array>
#include "BinarySearchTree.hpp"
#include "pugixml.hpp"
using namespace std;

//Class for a Bug object
class Bug
{
    private:
    
    //bug elements
    string id;
    string description;
    string impact;
    string date;

    public:

    //Constructor for Bug object
    Bug(string i, string de, string im, string da) : id(i), description(de), impact(im), date(da) {}

    //overload > operator
    bool operator > (Bug const &rhs)
    {
        //convert the impact level of the current bug and rhs bug to a sting name to an int value
        int bugImpact = ConvertImpactValue(impact);
        int bugImpactRHS = ConvertImpactValue(rhs.impact);

        //if the impacts are not equal to each other
        if(bugImpact != bugImpactRHS)
        {
            //return the bool comparison outcome of the impact levels
            return bugImpact < bugImpactRHS;
        }

        //otherwise, if the impacts are equal, bool comparison outcome of the dates
        return date > rhs.date;
    }

    //overload <>> operator
    bool operator < (Bug const &rhs)
    {
        //convert the impact level of the current bug and rhs bug to a sting name to an int value
        int bugImpact = ConvertImpactValue(impact);
        int bugImpactRHS = ConvertImpactValue(rhs.impact);

        //if the impacts are not equal to each other
        if(bugImpact != bugImpactRHS)
        {
            //return the bool comparison outcome of the impact levels
            return bugImpact > bugImpactRHS;
        }

        //otherwise, if the impacts are equal, bool comparison outcome of the dates
        return date < rhs.date;
    }

    //converts the impact level to a comparable int value
    int ConvertImpactValue(string i) const
    {
        if(i == "high") return 2;
        if(i == "medium") return 1;
        if(i == "low") return 0;
        return -1;
    }

    //getter for the Bug ID
    string GetId() const
    {
        return id;
    }

    //getter for the Bug description
    string GetDescription() const
    {
        return description;
    }

    //getter for the Bug impact
    string GetImpact() const
    {
        return impact;
    }

    //getter for the Bug date
    string GetDate() const
    {
        return date;
    }
};

//Class for a Developer object
class Developer
{
    private:

    //developer elements
    int id;
    array<Bug, 3> assignedBugs;

    public:

    //Constructor for Developer object
    Developer(int i, Bug hBug, Bug mBug, Bug lBug) : id(i), assignedBugs({hBug, mBug, lBug}) {}

    //getter for the Developer ID
    int GetId()
    {
        return id;
    }

    //getter for the desired Bug assigned to the Developer
    Bug GetBug(int index)
    {
        return assignedBugs.at(index);
    }
};

//Parses Bugs to a Binary Search Tree based upon their level of severity and date created from an input XML file
BinarySearchTree<Bug> parseBugsToBSTFromXML(string file)
{
    //instantiate a BST of Bug object type
   BinarySearchTree<Bug> BST;

   // Instantiate xml document variable and load passed in Bugs xml file
   pugi::xml_document inDoc;

   // Check to make sure the file was opened successfully
   if (!inDoc.load_file(file.c_str()))
   {
      cerr<<"Can't open "<<file<<"!"<<endl;
   }

   // Get the root node in the XML file
   pugi::xml_node root = inDoc.first_child();

   // Loop through all "Bug" elements in the XML file
   for (pugi::xml_node xmlBug : root.children("bug"))
   {
        // Retrieve the "id" attribute from the current <bug> node
        // Retrieve the "description" value from current <bug> node
        // Retrieve the "impact" value from current <bug> node
        // Retrieve the "created" value from current <bug> node
        string id = xmlBug.attribute("id").as_string();
        string description = xmlBug.child_value("description");
        string impact = xmlBug.child_value("impact");
        string date = xmlBug.child_value("created");

        // Instantiate Bug object using values retrieved from XML file
        Bug newBug(id, description, impact, date);

        //insert the Bug as data to the Bug BST
        BST.insert(newBug);
   }

   //return the populated Binary Search Tree of Bugs
   return BST;
}

//Assigns one (1) high impact bug, one (1) medium impact bug, and one (1) low impact bug with the earliest timestamp from a Binary Search Tree to a specified amount of developers
LinkedList<Developer> AssignBugs(int &amount, BinarySearchTree<Bug> &BST)
{
    //create a LL for developer objects
    LinkedList<Developer> dList;

    //create pointers to bug objects
    const Bug* b1;
    const Bug* b2;
    const Bug* b3;

    //amount of devs
    for (size_t index = 0; index < amount; index++)
    {
        //set the bug pointers to null pointers for initialization
        b1 = nullptr;
        b2 = nullptr;
        b3 = nullptr;

        // Create alias for BST iterator to make life easier
        //iterate through the sorted bugs BST
        using bstIter = typename BinarySearchTree<Bug>::Iterator;
        for(bstIter iter = BST.generateIterator(); iter.hasNext(); iter.next())
        {
            //find the first bug of high, medium, and low impact and set the bug pointers equal to that bug if the pointers are null
            if(iter.current()->GetImpact() == "high" && b1 == nullptr)
            {
                b1 = iter.current();
            }
            else if(iter.current()->GetImpact() == "medium" && b2 == nullptr)
            {
                b2 = iter.current();
            }
            else if(iter.current()->GetImpact() == "low" && b3 == nullptr)
            {
                b3 = iter.current();
            }

            //if one (1) high impact bug, one (1) medium impact bug, and one (1) low impact bug with the earliest timestamp have been initialized
            if(b1 != nullptr && b2 != nullptr && b3 != nullptr)
            {
                //break from the iteration loop
                break;
            }
        }

        //create new developer object and initialize its id and newly assigned bugs
        Developer newDev(index+1, *b1, *b2, *b3);

        //add developer to the developer linked list
        dList.pushBack(newDev);

        //remove the 3 assigned bugs from the binary search tree
        BST.remove(*b1);
        BST.remove(*b2);
        BST.remove(*b3);
    }
    
    //return the list of developers
    return dList;
}

//Loads assigned bugs from a list of developers to an output XML then loads the remaining unassigned bugs from a tree in order of earliest timestamp and severity to the same output XML file
void LoadToXML(BinarySearchTree<Bug> &BST, LinkedList<Developer> &LL)
{
    //create and open an output file named "OrderedBugs.xml"
    ofstream outFile;
    outFile.open("report.xml");

    // Check to make sure the file was opened successfully
    if (!outFile.is_open())
    {
        cerr<<"Can't open report.xml!"<<endl;
        return;
    }

    // write header for xml file
    outFile<<"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";

    //write root node
    outFile<<"<report>\n";

    ///////////////////////////
    //Loop through developer LL
    ///////////////////////////

    //iterate through the LL and write each bug assigned to a specifiic developer to the output XML file
    for (LinkedList<Developer>::Iterator iter = LL.generateIterator(); iter.hasNext(); iter.next())
    {
        //output the developers id
        outFile<<"\t<developer id=\""<<iter.current()->GetId()<<"\">\n";

        for (size_t index = 0; index < 3; index++)
        {
            outFile<<"\t\t<bug id=\""<<iter.current()->GetBug(index).GetId()<<"\">\n";
            outFile<<"\t\t\t<description>"<<iter.current()->GetBug(index).GetDescription()<<"</description>\n";
            outFile<<"\t\t\t<impact>"<<iter.current()->GetBug(index).GetImpact()<<"</impact>\n";
            outFile<<"\t\t\t<created>"<<iter.current()->GetBug(index).GetDate()<<"</created>\n";
            outFile<<"\t\t</bug>\n";
        }

        //end the bug output for the current developer
        outFile<<"\t</developer>\n";
    }

    //create remaining node
    outFile<<"\t<remaining>\n";

    ///////////////////////
    //Loop through Bug BST
    ///////////////////////

    //iterate through the BST and write each bug in order of severity and earliest date
    using bstIter = typename BinarySearchTree<Bug>::Iterator;
    for (bstIter iter = BST.generateIterator(); iter.hasNext(); iter.next())
    {
        outFile<<"\t\t<bug id=\""<<iter.current()->GetId()<<"\">\n";
        outFile<<"\t\t\t<description>"<<iter.current()->GetDescription()<<"</description>\n";
        outFile<<"\t\t\t<impact>"<<iter.current()->GetImpact()<<"</impact>\n";
        outFile<<"\t\t\t<created>"<<iter.current()->GetDate()<<"</created>\n";
        outFile<<"\t\t</bug>\n";
    }
    
    //close remaining node
    outFile<<"\t</remaining>\n";

    //end the root node
    outFile<<"</report>\n";

    // Close the output file
    outFile.close();
}


int main(int argc, char* argv[]) 
{
    // Simple validation for command line arguments
    if (argc != 3)
    {
        //assert that the user must enter a number of developers and an XML file path for parsing
        cerr<<"Must supply two command line arguments to program! (1. Number of developers. 2. XML file path containing bugs).\n";
        return 1;
    }

    //intialize two variables for the passed in CLA's based on the number of developers and XML file path
    int numDevs = stoi(argv[1]);
    string inFile = argv[2];

    //Instantiate a Binary Search Tree of Bug objects and parse the information from the XML file into the BST
    BinarySearchTree<Bug> bugBST = parseBugsToBSTFromXML(inFile);

    //Instantiate a Linked List of Developer objects and assign bugs to the specified number of developers from the Binary Search Tree of sorted bugs
    LinkedList<Developer> developerLL = AssignBugs(numDevs, bugBST);

    //Load the assigned bugs from the Linked List of Developers and unassigned bugs from the Binary Search Tree of Bugs into an organized output XML file
    LoadToXML(bugBST, developerLL);

    return 0;
}