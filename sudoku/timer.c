// updates timer events every 1 sec
#include <gtk/gtk.h>


static int passed_seconds = 0;                 
static gint timer = 0;
static int timer_running = FALSE;

void update_seconds(int);


gint TimerCallback(gpointer data) // Every second, this will be called using gtk_timeout_add and updates the time in the clock window.
{
    passed_seconds++; // second passes

    update_seconds(passed_seconds);

    return 1;
}


void StartTimer(int time_passed) // Starts up the time.  Happens when the user first inputs a cell.
{

    // If the timer isn't already running
    if (!timer_running) {

        passed_seconds = time_passed;

        // Call function 'TimerCallback' every 1000ms/1s
        timer = gtk_timeout_add(1000, TimerCallback, NULL);

        //set the timer state as running
        timer_running = TRUE;
    }
}


int StopTimer() // Stops the timer.  User either stoppedd or finished the game successfully.
{
    // If the timer is running.
    if (timer_running) {

        // Stop the timer.
        gtk_timeout_remove(timer);
 
        timer_running = FALSE;
    }
    return passed_seconds;
}
