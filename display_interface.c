#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int get_option(char *button1, char *button2){
    // Initialize ncurses
    initscr();
    start_color(); // Start color functionality
    init_pair(1, COLOR_GREEN, COLOR_BLACK); // Initialize a color pair
    init_pair(2, COLOR_BLUE, COLOR_BLACK); // Initialize a new color pair
    init_pair(3, COLOR_YELLOW, COLOR_BLACK); // Initialize a new color pair with yellow as the background color
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    int max_y = 0, max_x = 0;
    getmaxyx(stdscr, max_y, max_x); // Get the size of the terminal

    // Create the main window
    WINDOW* mainwin = newwin(max_y, max_x, 0, 0);
    box(mainwin, 0, 0);
    refresh();

    // Calculate the width of each button
    int button_width = max_x / 2;
    // Calculate the start position for the option strings
    int option1_start = (button_width - strlen(button1)) / 2;
    int option2_start = (button_width - strlen(button2)) / 2;

    // Create the dialogue text buttons
    WINDOW* buttonwin1 = newwin(max_y / 8, button_width, 2 * max_y / 9 + 4 * max_y / 6, 0);
    wbkgd(buttonwin1, COLOR_PAIR(1)); // Set the background color of the window
    box(buttonwin1, 0, 0);
    mvwprintw(buttonwin1, 1, option1_start, "%s", button1);
    wrefresh(buttonwin1);

    WINDOW* buttonwin2 = newwin(max_y / 8, button_width, 2 * max_y / 9 + 4 * max_y / 6, button_width);
    wbkgd(buttonwin2, COLOR_PAIR(1)); // Set the background color of the window
    box(buttonwin2, 0, 0);
    mvwprintw(buttonwin2, 1, option2_start, "%s", button2);
    wrefresh(buttonwin2);
    // Wait for user input
    int ch;
    int option = 0; // Start with option 0 selected
    while ((ch = getch()) != KEY_ENTER && ch != '\n' && ch != '\r') {
        if(ch == 'a'){
            option = 3;
            break;
        }
        else if(ch == 'b'){
            option = 4;
            break;
        }
        switch (ch) {
            case KEY_LEFT: // Move selection to the left
            case KEY_UP:
                option = 1;
                break;
            case KEY_RIGHT: // Move selection to the right
            case KEY_DOWN:
                option = 2;
                break;
        }

        // Highlight the selected button
        if (option == 1) {
            wattron(buttonwin1, A_REVERSE);
            mvwprintw(buttonwin1, 1, option1_start, "%s", button1);
            wattroff(buttonwin1, A_REVERSE);
            wrefresh(buttonwin1);

            mvwprintw(buttonwin2, 1, option2_start, "%s", button2);
            wrefresh(buttonwin2);
        } else if (option == 2) {
            wattron(buttonwin2, A_REVERSE);
            mvwprintw(buttonwin2, 1, option2_start, "%s", button2);
            wattroff(buttonwin2, A_REVERSE);
            wrefresh(buttonwin2);

            mvwprintw(buttonwin1, 1, option1_start, "%s", button1);
            wrefresh(buttonwin1);
        }
    }
    return option;
}

void display_interface(char *image, char *dialogue, int options, char *bag_item, int32_t affection_point) {
    // Initialize ncurses
    initscr();
    start_color(); // Start color functionality
    init_pair(1, COLOR_GREEN, COLOR_BLACK); // Initialize a color pair
    init_pair(2, COLOR_BLUE, COLOR_BLACK); // Initialize a new color pair
    init_pair(3, COLOR_YELLOW, COLOR_BLACK); // Initialize a new color pair with yellow as the background color
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    int max_y = 0, max_x = 0;
    getmaxyx(stdscr, max_y, max_x); // Get the size of the terminal

    // Create the main window
    WINDOW* mainwin = newwin(max_y, max_x, 0, 0);
    box(mainwin, 0, 0);
    refresh();

    // Clear the image area
    WINDOW* clearwin = newwin(4 * max_y / 6, 3 * max_x / 4, 0, 0);
    wclear(clearwin);
    wrefresh(clearwin);
    delwin(clearwin);

    // Display the image
    if(fopen(image, "r") == NULL){}
    else{
        char command[512];
        snprintf(command, sizeof(command), "img2sixel %s", image); // Construct the command string
        system(command); // Execute the command
    }

    // Create the bag window
    WINDOW* bagwin = newwin(max_y / 3, max_x / 4, 0, 3 * max_x / 4);
    wattron(bagwin, COLOR_PAIR(2)); // Turn on color attribute
    box(bagwin, 0, 0); // Draw the box with the color attribute on
    wattroff(bagwin, COLOR_PAIR(2)); // Turn off color attribute
    mvwprintw(bagwin, 0, 2, "Bag");
    mvwprintw(bagwin, 2, 2, "%s", bag_item);
    wrefresh(bagwin);

    // Create the affection point window
    WINDOW* char1win = newwin(max_y / 3, max_x / 4, max_y / 3, 3 * max_x / 4);
    box(char1win, 0, 0);
    mvwprintw(char1win, 0, 2, "Affection Point");
    mvwprintw(char1win, 2, 2, "%d", affection_point);
    wrefresh(char1win);

    // Create the dialogue line window
    WINDOW* dialogwin = newwin(2 * max_y / 9, max_x, 4 * max_y / 6, 0);
    box(dialogwin, 0, 0);
    mvwprintw(dialogwin, 2 * max_y / 9 - 3, max_x - 24, "[press 'a' to continue]");
    mvwprintw(dialogwin, 2 * max_y / 9 - 2, max_x - 24, "[press 'b' to open bag]");
    mvwprintw(dialogwin, 1, 1, "%s", dialogue);
    wbkgd(dialogwin, COLOR_PAIR(3));
    wrefresh(dialogwin);
}
