#include "cshell.h"

int builtin_jobs(char **args)
{
    (void)args; // Suppress unused parameter warning
    
    if (!g_data.background_jobs || g_data.job_count == 0)
    {
        dl_putendl_fd("No background jobs", STDOUT_FILENO);
        return (EXIT_SUCCESS);
    }
    
    // Display background jobs
    for (int i = 0; i < g_data.job_count; i++)
    {
        if (g_data.background_jobs[i].pid > 0)
        {
            // Format: [job_number] status command
            char *job_num = dl_itoa(i + 1);
            if (job_num)
            {
                dl_putchar_fd('[', STDOUT_FILENO);
                dl_putstr_fd(job_num, STDOUT_FILENO);
                dl_putchar_fd(']', STDOUT_FILENO);
                free(job_num);
            }
            
            // Job status
            if (g_data.background_jobs[i].status == JOB_RUNNING)
                dl_putstr_fd(" Running", STDOUT_FILENO);
            else if (g_data.background_jobs[i].status == JOB_STOPPED)
                dl_putstr_fd(" Stopped", STDOUT_FILENO);
            else if (g_data.background_jobs[i].status == JOB_COMPLETED)
                dl_putstr_fd(" Done", STDOUT_FILENO);
            else
                dl_putstr_fd(" Unknown", STDOUT_FILENO);
            
            // Job command
            if (g_data.background_jobs[i].command)
            {
                dl_putchar_fd(' ', STDOUT_FILENO);
                dl_putstr_fd(g_data.background_jobs[i].command, STDOUT_FILENO);
            }
            
            dl_putchar_fd('\n', STDOUT_FILENO);
        }
    }
    
    return (EXIT_SUCCESS);
}

int add_background_job(pid_t pid, char *command)
{
    if (!g_data.background_jobs)
    {
        // Initialize background jobs array
        g_data.background_jobs = dl_calloc(MAX_JOBS, sizeof(t_job));
        if (!g_data.background_jobs)
            return (-1);
        g_data.job_count = 0;
    }
    
    // Find empty slot
    for (int i = 0; i < MAX_JOBS; i++)
    {
        if (g_data.background_jobs[i].pid == 0)
        {
            g_data.background_jobs[i].pid = pid;
            g_data.background_jobs[i].command = dl_strdup(command);
            g_data.background_jobs[i].status = JOB_RUNNING;
            g_data.background_jobs[i].start_time = time(NULL);
            
            if (g_data.job_count < i + 1)
                g_data.job_count = i + 1;
            
            return (i);
        }
    }
    
    return (-1); // No available slots
}

int remove_background_job(pid_t pid)
{
    if (!g_data.background_jobs)
        return (-1);
    
    for (int i = 0; i < g_data.job_count; i++)
    {
        if (g_data.background_jobs[i].pid == pid)
        {
            // Clear job
            if (g_data.background_jobs[i].command)
                free(g_data.background_jobs[i].command);
            
            g_data.background_jobs[i].pid = 0;
            g_data.background_jobs[i].command = NULL;
            g_data.background_jobs[i].status = JOB_NONE;
            g_data.background_jobs[i].start_time = 0;
            
            // Update job count
            while (g_data.job_count > 0 && 
                   g_data.background_jobs[g_data.job_count - 1].pid == 0)
                g_data.job_count--;
            
            return (i);
        }
    }
    
    return (-1); // Job not found
}

void update_job_status(pid_t pid, int status)
{
    if (!g_data.background_jobs)
        return;
    
    for (int i = 0; i < g_data.job_count; i++)
    {
        if (g_data.background_jobs[i].pid == pid)
        {
            if (WIFEXITED(status))
            {
                g_data.background_jobs[i].status = JOB_COMPLETED;
                g_data.background_jobs[i].exit_status = WEXITSTATUS(status);
            }
            else if (WIFSIGNALED(status))
            {
                g_data.background_jobs[i].status = JOB_COMPLETED;
                g_data.background_jobs[i].exit_status = 128 + WTERMSIG(status);
            }
            else if (WIFSTOPPED(status))
            {
                g_data.background_jobs[i].status = JOB_STOPPED;
            }
            break;
        }
    }
}
