#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>

#include "verifications.h"
#include "solve.h"
#include "files.h"
#include "generate.h"
#include "save_file.h"
#include "digits.h"

static int level = 2;
static int mode_edition = 0;
static int grid[9][9];
static int mistakes[9][9]; // grid copy to save the mistakes
static int blocked_cells[9][9]; // coordinations of blocked cells
static int initial_grid[9][9];
static int temp_grid[9][9];

// Main gtk widgets for the project:
static GtkWidget* window;
static GtkWidget* grid_screen[9][9];
GtkWidget *label_time;            /* --- Label showing time passed --- */

GtkWidget *verticalBox;// boxe that will contain the different widgets


// Timer functions
extern void StartTimer (int time_passed);
extern int StopTimer ();

gpointer digits[] = { 
    NULL, 
    xpm_one, 
    xpm_two,
    xpm_three,
    xpm_four,
    xpm_five,
    xpm_six,
    xpm_seven,
    xpm_eight,
    xpm_nine
};

// function allowing to synchronize the graphic part and the algorithmic part
/*This function allows the transfer of the numbers entered by the user in the grid_screen[][] widget to the integer array grid[][] 
which will be used in the algorithms as well as the possibility of blocking the grid boxes*/
void grid_display_bridge(int direction, int blocking) //it takes the direction of the conversion and the blocking of the boxes if necessary
{
    if(direction==1) // display to grid
    {
        int i,j;
        gchar *p;// only characters can be diplayed on the screen, so a conversion is necessary
        for (i=0;i<9;i++)
        {
            for (j=0;j<9;j++)
            {
                p = (char*) gtk_entry_get_text(GTK_ENTRY(grid_screen[i][j])); // we put the displayed character string in p
                grid[i][j]= atoi(p); // convert to integer and insert into the grid
            }
        }
    }
    else if (direction == 0) // grid to display
    {
        int i,j;
        char number_c[2];// only characters can be diplayed on the screen, so a conversion is necessary
        for (i=0;i<9;i++)
        {
            for (j=0;j<9;j++)
            {   
                gtk_entry_set_editable (GTK_ENTRY(grid_screen[i][j]),TRUE);// unblock the grid cell
                snprintf(number_c, 2, "%i", grid[i][j]);// convert the integers into characters
                gtk_entry_set_text(GTK_ENTRY(grid_screen[i][j]), number_c);// display the characters
                // anything that is not 0 initially is blocked:
                if ((blocking==1) && (grid[i][j]!=0 ))
                {
                    gtk_entry_set_editable (GTK_ENTRY(grid_screen[i][j]),FALSE);
                    blocked_cells[i][j] = 1;
                }
                else if ((mistakes[i][j]!=0) && (blocked_cells[i][j]!=1))
                {
                    gtk_entry_set_max_length(GTK_ENTRY(grid_screen[i][j]),2);
                    snprintf(number_c, 3, "%i*", grid[i][j]);
                    gtk_entry_set_text(GTK_ENTRY(grid_screen[i][j]), number_c);
                }
                else if (mistakes[i][j]==0)
                    gtk_entry_set_max_length(GTK_ENTRY(grid_screen[i][j]),1); // reset max length
            }
        }
    }
}

/*
  Refresh the seconds display in the toolbar.
 
  nSeconds - how many seconds to display.
 */
void update_seconds(int nSeconds)
{
    char buffer[44];

    // Change the label to show new time
    sprintf (buffer, "Time: %d", nSeconds);
    gtk_label_set (GTK_LABEL (label_time), buffer);
}


// Menu management function
// We recover the signal sent by the menu item and we compare its label with different
// character strings to find out on which item the user clicked and thus launch the correct function
static void menu_event(GtkWidget *widget, gpointer data)
{
    /* Verify the grid */
    // fill the grid with the elements displayed on the screen and then test if this new grid is ok with the rules of sudoku 
    // by calling the function verif_valid. We then open a dialog box depending on the result.
    if (strcmp(gtk_menu_item_get_label(GTK_MENU_ITEM(widget)),g_locale_to_utf8("Verify the grid", -1, NULL, NULL, NULL))==0)
    {
        mode_edition=0;
        GtkWidget *finish;
        int finished, valid;

        grid_display_bridge(1,0);// synch grid with the display
        valid = verif_valid(grid, mistakes); // test if the entry is ok with the rules of sudoku
        verif_finish(grid, &finished); // check if the grid is finished

        // Depending on the situation, three messages are possible
        if (finished == 1 && valid == 1)
        {
            int time_passed = StopTimer ();        
            char win_message[100];
            sprintf(win_message, "\nWell done, the grid is solved!\nTime spent: %d seconds", time_passed);
            finish = gtk_message_dialog_new(GTK_WINDOW(window),GTK_DIALOG_MODAL,GTK_MESSAGE_INFO,GTK_BUTTONS_OK, 
            g_locale_to_utf8(win_message, -1, NULL, NULL, NULL) );
        }
        else if ( valid == 1 ) 
            finish = gtk_message_dialog_new(GTK_WINDOW(window),GTK_DIALOG_MODAL,GTK_MESSAGE_INFO,GTK_BUTTONS_OK, 
            g_locale_to_utf8("\nNot completed yet, keep going!", -1, NULL, NULL, NULL) );
        else 
            finish = gtk_message_dialog_new(GTK_WINDOW(window),GTK_DIALOG_MODAL,GTK_MESSAGE_WARNING,GTK_BUTTONS_OK, "\nThe current grid is wrong, try again!\nYour mistakes are marked with *." );

        grid_display_bridge(0, 0);// show mistakes

        gtk_widget_show_all(finish);
        gtk_dialog_run(GTK_DIALOG(finish));
        gtk_widget_destroy(finish);
    }

    /* Generate a grid (Restart)*/
    // compare the label:
    else if(strcmp(gtk_menu_item_get_label(GTK_MENU_ITEM(widget)),g_locale_to_utf8("Generate a grid", -1, NULL, NULL, NULL))==0)
    {
        mode_edition=0;
        // Stop the timer
        StopTimer ();
        update_seconds (0);
        generate(grid, level); // we call to generate grid by sending the desired level (from 1 to 3)
        copy_grid(grid,initial_grid);// save the initial grid
        grid_display_bridge(0,1);// block the generated boxes
        StartTimer(0);
    }

    /*Empty grid*/
    // Resets the grid to 0 so that the user can create their own
    else if(strcmp(gtk_menu_item_get_label(GTK_MENU_ITEM(widget)),"Empty grid") == 0 )
    {

        int i,j;
        for (i=0;i<9;i++)
        {
            for (j=0;j<9;j++)
            {
                grid[i][j]=0;
                gtk_entry_set_editable (GTK_ENTRY(grid_screen[i][j]),TRUE);// we make sure that the boxes are editable (in the case where we click after opening the grid)
                gtk_entry_set_text(GTK_ENTRY(grid_screen[i][j]), "0");// show 0 as the cell number
            }
        }
        copy_grid(grid,initial_grid);
        mode_edition=1;
        // Stop the timer
        StopTimer ();
        update_seconds (0);
    }

    /*Open a grid */
    // Allows the user to open a file containing a grid and display it on the screen
    else if(strcmp(gtk_menu_item_get_label(GTK_MENU_ITEM(widget)),"Open a grid ")==0)
    {
        mode_edition=0;
        GtkWidget *dialog_box=NULL;// create a widget which will be initialized in the file choice dialog box
        dialog_box = gtk_file_chooser_dialog_new("Open a grid ", GTK_WINDOW(window),GTK_FILE_CHOOSER_ACTION_OPEN,GTK_STOCK_OK, GTK_RESPONSE_OK,GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,NULL);
        gtk_widget_show_all(dialog_box);// display the dialog box widget

        // test the user's response, if they click ok, we open the request file and display it on the screen
        gint reponse=gtk_dialog_run(GTK_DIALOG(dialog_box));
        if(reponse == GTK_RESPONSE_OK)
        {
            char *file_name;// store the name of the file that the user has chosen
            file_name = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog_box)); // get the file name

            int time_passed = read_file(grid, file_name);// the read_file function is sent to the grid as well as the name. The file is read and the grid is filled
            grid_display_bridge(0,1);
            StartTimer (time_passed);
            update_seconds(time_passed);
        }

        gtk_widget_destroy(dialog_box);// destroy the dialog box
        copy_grid(grid, initial_grid);
    }

    /*Save the grid*/
    // Allows the user to save a grid displayed on the screen in a file
    else if(strcmp(gtk_menu_item_get_label(GTK_MENU_ITEM(widget)),"Save")==0)
    {
        mode_edition=0;
        grid_display_bridge(1,0);// synchronize the grid with the display
        if (verif_valid(grid, mistakes)) // check that the grid is valid for saving
        {
            int time_passed = StopTimer();
            GtkWidget *dialog_box=NULL;// the widget will be initialized in the dialog box of choice of file to save
            dialog_box = gtk_file_chooser_dialog_new ("Save the file", NULL,GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);
            gtk_widget_show_all(dialog_box);
            gint reponse=gtk_dialog_run(GTK_DIALOG(dialog_box));// as before we check the response
            if(reponse == GTK_RESPONSE_ACCEPT)
            {
                char *file_name;
                file_name = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog_box));
                grid_display_bridge(1,0);
                save_grid(grid, file_name, time_passed); // save the grid on the file
            }
            gtk_widget_destroy(dialog_box);
        }
        else // if the grif is invalid we display a widget that informs the user of the problem so that he can correct his entry
        {
            GtkWidget *invalid;
            invalid = gtk_message_dialog_new(GTK_WINDOW(window),GTK_DIALOG_MODAL,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK, g_locale_to_utf8("\nThere is an error in the grid!", -1, NULL, NULL, NULL) );
            gtk_widget_show_all(invalid);
            gtk_dialog_run(GTK_DIALOG(invalid)); // start the dialogue
            gtk_widget_destroy(invalid);
        }
    }

    /*Solve*/
    // Allows the user to solve the current grid, if it does not contain a fault or to indicate if it does.
    else if(strcmp(gtk_menu_item_get_label(GTK_MENU_ITEM(widget)),g_locale_to_utf8("Solve the current grid", -1, NULL, NULL, NULL))==0)
    {

        grid_display_bridge(1,0);
        if(mode_edition ==1) 
            copy_grid(grid, initial_grid); // to prevent solving of an empty grid
        copy_grid(grid, temp_grid);
        solve(temp_grid);
        solve(initial_grid);
        if (verif_valid(grid, mistakes) && compare_grids(initial_grid, temp_grid) == 1)
        {
            solve(grid);// if the grid does not include user error and is solvable, we solve it
            grid_display_bridge(0,1);
        }
        else if (verif_valid(grid, mistakes)==1 && compare_grids(initial_grid, temp_grid) ==0)
        {
            GtkWidget *invalid;//we create a widget which will be initialized in an information dialog box, for the conversion of labels
            invalid = gtk_message_dialog_new(GTK_WINDOW(window),GTK_DIALOG_MODAL,GTK_MESSAGE_WARNING,GTK_BUTTONS_OK, 
                g_locale_to_utf8("\nThe grid respects the rules of sudoku \n but it does not correspond to the solution, so it is not solveable.", -1, NULL, NULL, NULL) );
            gtk_widget_show_all(invalid);
            gtk_dialog_run(GTK_DIALOG(invalid));
            gtk_widget_destroy(invalid);
        }
        else // otherwise we display a widget that informs the user of the problem so that they can correct their entry
        {
            GtkWidget *invalid;
            invalid = gtk_message_dialog_new(GTK_WINDOW(window),GTK_DIALOG_MODAL,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK, 
            g_locale_to_utf8("\nThere is an error in the grid. \nWith these entries it is unsolvable.", -1, NULL, NULL, NULL) );
            gtk_widget_show_all(invalid);
            gtk_dialog_run(GTK_DIALOG(invalid));
            gtk_widget_destroy(invalid);
        }

    }

    /*About the game*/
    else if(strcmp(gtk_menu_item_get_label(GTK_MENU_ITEM(widget)),"About")==0)
    {
        GtkWidget *About;
        About = gtk_message_dialog_new(GTK_WINDOW(window),GTK_DIALOG_MODAL,GTK_MESSAGE_INFO,GTK_BUTTONS_OK, "Sudoku game designed using C and GTK+ \n By Hossein (meyhossi@gmail.com)" );
        gtk_widget_show_all(About);
        gtk_dialog_run(GTK_DIALOG(About));
        gtk_widget_destroy(About);
    }
    // GET the difficulty choice of the user
    else if(strcmp(gtk_menu_item_get_label(GTK_MENU_ITEM(widget)),"Level 0")==0) 
        level=0;
    else if(strcmp(gtk_menu_item_get_label(GTK_MENU_ITEM(widget)),"Level 1")==0) 
        level=1;
    else if(strcmp(gtk_menu_item_get_label(GTK_MENU_ITEM(widget)),"Level 2")==0) 
        level=2;
    else if(strcmp(gtk_menu_item_get_label(GTK_MENU_ITEM(widget)),"Level 3")==0) 
        level=3;
    /*Exit the game*/
    else if(strcmp(gtk_menu_item_get_label(GTK_MENU_ITEM(widget)),"Exit")==0) 
        gtk_main_quit();
}

int main (int argc, char *argv[])
{

    // Creation of the required widgets

    GtkWidget *horizontalBox;
    GtkWidget *seperator;
    GtkWidget *button; // solve button
    GtkWidget *file_menu, *help_menu, *leveL_menu; // menu widgets
    GtkWidget *toolbar;
    GtkWidget *menu_item;

    // Creation and initialization of the labels of the menu bar

    const char* items[] = {"Verify the grid", "Generate a grid", "Empty grid", "Open a grid ", "Save", "Exit"};
    const char* help_items[] = {"Solve the current grid", "About"};
    const char* levels[] = {"Level 1", "Level 2", "Level 3"};

    const gchar* labelConv; // Needed for converting labels to utf8 and solving the accented word problem

    int i, j;

    //int grid[9][9] = {0}; // initializing the grid with 0 value

    gtk_init(&argc, &argv); // initialize gtk

    // Create window

    window=gtk_window_new(GTK_WINDOW_TOPLEVEL);// initialize the main window of the program
    gtk_window_set_title(GTK_WINDOW(window), "Sudoku");
    gtk_window_set_icon_from_file(GTK_WINDOW(window),"icon.ico",NULL); // insert an icon for the program
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE); // user resizing is not allowed

    // we initialize the vertical box widget
    verticalBox = gtk_vbox_new(0,10);

    // Create the menu bar and the menus that form it
    // initialize the menu bar and menu widgets:
    toolbar=gtk_menu_bar_new(); 
    file_menu=gtk_menu_new(); 
    help_menu=gtk_menu_new();
    leveL_menu=gtk_menu_new();

    //menu file
    menu_item=gtk_menu_item_new_with_label("Game");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), file_menu);// define file_menu as a submenu
    gtk_menu_shell_append(GTK_MENU_SHELL(toolbar), menu_item);// add the items from top to bottom

    // use a loop to define the elements and save time
    for ( i=0; i <6; i++)
    {
        labelConv = g_locale_to_utf8(items[i], -1, NULL, NULL, NULL); // Convert strings to utf8
        menu_item=gtk_menu_item_new_with_label(labelConv); // initialize the menu item and assign it a label
        gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), menu_item); // add the items in the menu from top to bottom
        g_signal_connect(menu_item,"activate",G_CALLBACK(menu_event), NULL);// we connect the signal to the menu_event callback function (see below)
    }

    //menu level
    menu_item=gtk_menu_item_new_with_label("Level");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), leveL_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(toolbar), menu_item);
    for ( i=0; i <3 ; i++)
    {
        labelConv = g_locale_to_utf8(levels[i], -1, NULL, NULL, NULL);
        menu_item=gtk_menu_item_new_with_label(labelConv);
        gtk_menu_shell_append(GTK_MENU_SHELL(leveL_menu), menu_item);
        g_signal_connect(menu_item,"activate",G_CALLBACK(menu_event), NULL);
    }

    //menu help
    menu_item=gtk_menu_item_new_with_label("Help");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), help_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(toolbar), menu_item);
    for ( i=0; i <2 ; i++)
    {
        labelConv = g_locale_to_utf8(help_items[i], -1, NULL, NULL, NULL);
        menu_item=gtk_menu_item_new_with_label(labelConv);
        gtk_menu_shell_append(GTK_MENU_SHELL(help_menu), menu_item);
        g_signal_connect(menu_item,"activate",G_CALLBACK(menu_event), NULL);
    }

    gtk_box_pack_start(GTK_BOX(verticalBox), toolbar, 0,0,0); // insert the menu bar in the vertical box from the top

    //Grid creation
    for (i =0; i<9; i++)
    {
        horizontalBox = gtk_hbox_new(0,10); // initialize the horizontal box widget
        for (j=0;j<9;j++)
        {
                grid_screen[i][j]=gtk_entry_new();// initialize the cells
                gtk_entry_set_max_length(GTK_ENTRY(grid_screen[i][j]),1); // limit the size of gtk_entry to a single element
                gtk_widget_set_size_request(grid_screen[i][j],30,30); // setting the size of gtk_entry
                
                // We will then create the regions by artificially separating the boxes
                gtk_box_pack_start(GTK_BOX(horizontalBox),grid_screen[i][j],0,0,0);

                if((j+1)%3==0 && j < 8) // so as not to have a 3rd separator
                { 
                    seperator = gtk_vseparator_new(); // vertical seperator 
                    gtk_box_pack_start(GTK_BOX(horizontalBox),seperator,0,0,0);
                }
        }
        gtk_box_pack_start(GTK_BOX(verticalBox),horizontalBox,0,0,0);
        if((i+1)%3==0 && i < 8)
        {    
            seperator = gtk_hseparator_new();
            gtk_box_pack_start(GTK_BOX(verticalBox),seperator,0,0,0);
        }
    }

    // Add label with the time passed
    label_time = gtk_label_new ("");
    gtk_box_pack_start (GTK_BOX (verticalBox), label_time, FALSE, FALSE, 0);
    gtk_widget_show (label_time);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL); // connect the signal to the gtk_main_quit function to allow the program to stop

    gtk_container_add(GTK_CONTAINER(window), verticalBox);// add the vertical box to the window
    gtk_widget_show_all(window); // display the window
    gtk_main();

    return EXIT_SUCCESS;
}
