#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fstream>
#include "General.cpp"
using namespace std;

void write_pipe_input(vector<double> values_prv)
{
    // Define the name of the named pipe.
    const char *pipe_name = "my_pipe";

    // Create the named pipe with read and write permissions.
    int res = mkfifo(pipe_name, 0666);

    // Check if the named pipe was created successfully.
    if (res < 0)
        cerr << "Error creating pipe!\n";

    // Open the named pipe for reading and writing.
    int fd = open(pipe_name, O_RDWR);

    // Check if the named pipe was opened successfully.
    if (fd < 0)
        cerr << "Error opening pipe for read/write!\n";

    // Initialize the message string.
    string message = {};

    // Loop through the input vector and write each element to the named pipe.
    for (int i = 1; i < values_prv.size(); i++)
    {
        // Convert the current element to a string and append a comma separator.
        message = to_string(values_prv[i]);
        message += ',';

        // Write the current message to the named pipe.
        if (write(fd, message.c_str(), message.length()) <= 0)
            cout << "Writing failed" << endl;

        // Reset the message string.
        message = {};
    }

    // Write a null character to the named pipe to signal the end of the message.
    const char ch = '\0';
    write(fd, &ch, 1);

    // Print the final value of the message string.
    cout << message << endl;
}

int main(int argc, char *argv[])
{
    // Check if the number of command line arguments is correct
    if (argc != 5)
    {
        // Print the correct usage and exit the program
        fprintf(stderr, "Usage: program2 int_var1 int_var2 bool_var\n");
        exit(EXIT_FAILURE);
    }

    // Convert the command line arguments to variables
    int size = atoi(argv[1]);
    int count = atoi(argv[2]);
    bool dir = atoi(argv[3]) != 0;
    bool rep = atoi(argv[3]) != 0;

    // Initialize some variables
    char var1_str[10]{}, var2_str[10]{}, var3_str[6]{}, var4_str[6]{};

    // If dir is true, perform the following steps
    if (dir)
    {
        // Remove any existing pipe and create a new one
        unlink("my_pipe");
        const char *pipe_name = "my_pipe";
        int res = mkfifo(pipe_name, 0666);
        if (res < 0)
            cerr << "Error creating pipe!\n";

        // Open the pipe for reading and writing
        int fd = open(pipe_name, O_RDWR);
        if (fd < 0)
            cerr << "Error opening pipe for read/write!\n";

        // Read input data from a file and send it through the pipe
        int input_size = 5;
        cout << " Enter the Number of Inputs you want Neural Network to perform on : ";
        cin >> input_size;
        double *input = getData("temp.txt", input_size, ',');
        cout << " The Inputs are : ";
        for (int i = 0; i < input_size; i++)
            cout << input[i] << ' ';
        cout << endl;

        string message = {};
        for (int i = 0; i < input_size; i++)
        {
            message = to_string(input[i]);
            message += ',';
            cout << message;
            if (write(fd, message.c_str(), message.length()) <= 0)
                cout << "Writing failed" << endl;
            message = {};
        }
        const char ch = '\0';
        write(fd, &ch, 1);

        // Convert variables to strings and prepare for passing to execv
        snprintf(var1_str, sizeof(var1_str), "%d", size);
        int temp = 1;
        snprintf(var2_str, sizeof(var2_str), "%d", temp);
        dir = true;
        snprintf(var3_str, sizeof(var3_str), "%d", dir);
        rep = true;
        snprintf(var4_str, sizeof(var4_str), "%d", rep);
        const char *const argv[] = {"hidden", var1_str, var2_str, var3_str, var4_str, NULL};

        // Call execv to run another program
        execv("hidden", const_cast<char *const *>(argv));

        // If execv fails, print an error message and exit
        perror("execv");
        exit(EXIT_FAILURE);
    }
    else
    {
        // Output a message indicating that the input layer has been reached backwards
        cout << " Input layer reached backwards " << endl;

        // Output a message indicating that the program is returning to the input layer
        cout << " BAck to Input layer " << endl;

        // Read the values generated by the neural network from the pipe
        vector<double> values_prv = read_pipe();

        // Output the value generated by the neural network
        cout << " The Output Generated was : " << values_prv[0] << endl;

        // Output the new generated inputs
        cout << " The new generated Inputs are : " << values_prv[1] << ' ' << values_prv[2] << endl;

        // Write the values generated by the neural network back into the pipe
        write_pipe_input(values_prv);

        // Convert some variables to strings for passing to the next program as command line arguments
        snprintf(var1_str, sizeof(var1_str), "%d", size);
        int temp = 1;
        snprintf(var2_str, sizeof(var2_str), "%d", temp);
        dir = true;
        snprintf(var3_str, sizeof(var3_str), "%d", dir);
        rep = false;
        snprintf(var4_str, sizeof(var4_str), "%d", rep);

        // Launch the next program in the pipeline
        const char *const argv[] = {"hidden", var1_str, var2_str, var3_str, var4_str, NULL};
        execv("hidden", const_cast<char *const *>(argv));

        // Print an error message and exit if the program failed to launch
        perror("execv");
        exit(EXIT_FAILURE);
    }
}