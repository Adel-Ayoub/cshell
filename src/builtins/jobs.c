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

int builtin_fg(char **args)
{
    int job_id = -1;  // Initialize to invalid value
    
    if (!args || !args[1])
    {
        // No job specified, bring the most recent job to foreground
        if (!g_data.background_jobs || g_data.job_count == 0)
        {
            dl_putendl_fd("fg: no current job", STDERR_FILENO);
            return (EXIT_FAILURE);
        }
        
        // Find the most recent job
        for (int i = g_data.job_count - 1; i >= 0; i--)
        {
            if (g_data.background_jobs[i].pid > 0 && 
                g_data.background_jobs[i].status == JOB_RUNNING)
            {
                job_id = i;
                break;
            }
        }
        
        // Check if we found a job
        if (job_id == -1)
        {
            dl_putendl_fd("fg: no current job", STDERR_FILENO);
            return (EXIT_FAILURE);
        }
    }
    else
    {
        // Parse job ID from argument
        if (args[1][0] == '%')
            job_id = dl_atoi(args[1] + 1) - 1;
        else
            job_id = dl_atoi(args[1]) - 1;
    }
    
    // Validate job ID
    if (job_id < 0 || job_id >= g_data.job_count || 
        g_data.background_jobs[job_id].pid <= 0)
    {
        dl_putstr_fd("fg: job not found: ", STDERR_FILENO);
        dl_putstr_fd(args[1] ? args[1] : "current", STDERR_FILENO);
        dl_putchar_fd('\n', STDERR_FILENO);
        return (EXIT_FAILURE);
    }
    
    // Check if job is running
    if (g_data.background_jobs[job_id].status != JOB_RUNNING)
    {
        dl_putstr_fd("fg: job ", STDERR_FILENO);
        char *job_str = dl_itoa(job_id + 1);
        dl_putstr_fd(job_str, STDERR_FILENO);
        free(job_str);
        dl_putendl_fd(" is not running", STDERR_FILENO);
        return (EXIT_FAILURE);
    }
    
    // Bring job to foreground
    pid_t pid = g_data.background_jobs[job_id].pid;
    
    // Set process group to foreground
    if (setpgid(pid, getpgid(0)) == -1)
    {
        perror("fg: setpgid failed");
        return (EXIT_FAILURE);
    }
    
    // Send SIGCONT to wake up stopped jobs
    if (kill(pid, SIGCONT) == -1)
    {
        perror("fg: SIGCONT failed");
        return (EXIT_FAILURE);
    }
    
    // Wait for the job to complete
    int status;
    if (waitpid(pid, &status, WUNTRACED) == -1)
    {
        perror("fg: waitpid failed");
        return (EXIT_FAILURE);
    }
    
    // Update job status
    update_job_status(pid, status);
    
    // Remove completed job
    if (WIFEXITED(status) || WIFSIGNALED(status))
    {
        remove_background_job(pid);
    }
    
    return (WEXITSTATUS(status));
}

int builtin_bg(char **args)
{
    int job_id = -1;  // Initialize to invalid value
    
    if (!args || !args[1])
    {
        // No job specified, resume the most recent stopped job
        if (!g_data.background_jobs || g_data.job_count == 0)
        {
            dl_putendl_fd("bg: no current job", STDERR_FILENO);
            return (EXIT_FAILURE);
        }
        
        // Find the most recent stopped job
        for (int i = g_data.job_count - 1; i >= 0; i--)
        {
            if (g_data.background_jobs[i].pid > 0 && 
                g_data.background_jobs[i].status == JOB_STOPPED)
            {
                job_id = i;
                break;
            }
        }
        
        // Check if we found a job
        if (job_id == -1)
        {
            dl_putendl_fd("bg: no current job", STDERR_FILENO);
            return (EXIT_FAILURE);
        }
    }
    else
    {
        // Parse job ID from argument
        if (args[1][0] == '%')
            job_id = dl_atoi(args[1] + 1) - 1;
        else
            job_id = dl_atoi(args[1]) - 1;
    }
    
    // Validate job ID
    if (job_id < 0 || job_id >= g_data.job_count || 
        g_data.background_jobs[job_id].pid <= 0)
    {
        dl_putstr_fd("bg: job not found: ", STDERR_FILENO);
        dl_putstr_fd(args[1] ? args[1] : "current", STDERR_FILENO);
        dl_putchar_fd('\n', STDERR_FILENO);
        return (EXIT_FAILURE);
    }
    
    // Check if job is stopped
    if (g_data.background_jobs[job_id].status != JOB_STOPPED)
    {
        dl_putstr_fd("bg: job ", STDERR_FILENO);
        char *job_str = dl_itoa(job_id + 1);
        dl_putstr_fd(job_str, STDERR_FILENO);
        free(job_str);
        dl_putendl_fd(" is not stopped", STDERR_FILENO);
        return (EXIT_FAILURE);
    }
    
    // Resume job in background
    pid_t pid = g_data.background_jobs[job_id].pid;
    
    // Set process group to background
    if (setpgid(pid, getpgid(0)) == -1)
    {
        perror("bg: setpgid failed");
        return (EXIT_FAILURE);
    }
    
    // Send SIGCONT to resume the job
    if (kill(pid, SIGCONT) == -1)
    {
        perror("bg: SIGCONT failed");
        return (EXIT_FAILURE);
    }
    
    // Update job status to running
    g_data.background_jobs[job_id].status = JOB_RUNNING;
    
    // Display job info
    dl_putchar_fd('[', STDOUT_FILENO);
    char *job_str = dl_itoa(job_id + 1);
    dl_putstr_fd(job_str, STDOUT_FILENO);
    free(job_str);
    dl_putstr_fd("] ", STDOUT_FILENO);
    
    if (g_data.background_jobs[job_id].command)
        dl_putstr_fd(g_data.background_jobs[job_id].command, STDOUT_FILENO);
    
    dl_putendl_fd(" &", STDOUT_FILENO);
    
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
