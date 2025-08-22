#include "cshell.h"
#include <dirent.h>
#include <sys/stat.h>

int wild_card_check(char *pattern)
{
    if (!pattern)
        return (0);
    
    // Check if pattern contains wildcards
    while (*pattern)
    {
        if (*pattern == '*' || *pattern == '?' || *pattern == '[')
            return (1);
        pattern++;
    }
    return (0);
}

int expand_wildcards(void)
{
    if (!g_data.args_array || !g_data.args_array[0])
        return (0);
    
    int i = 0;
    int expanded_count = 0;
    
    // First pass: count how many arguments we'll have after expansion
    while (g_data.args_array[i])
    {
        if (g_data.args_array[i] && wild_card_check(g_data.args_array[i]))
        {
            // Don't expand the first argument (command name)
            if (i == 0)
            {
                expanded_count++;
            }
            else
            {
                char **expanded = expand_wildcard_pattern(g_data.args_array[i]);
                if (expanded)
                {
                    int j = 0;
                    while (expanded[j])
                    {
                        expanded_count++;
                        j++;
                    }
                    free_string_array(expanded);
                }
                else
                {
                    // No matches found, keep original pattern
                    expanded_count++;
                }
            }
        }
        else
        {
            expanded_count++;
        }
        i++;
    }
    
    if (expanded_count == 0)
        return (0);
    
    // Allocate new array for expanded arguments
    char **new_args = dl_calloc(expanded_count + 1, sizeof(char *));
    if (!new_args)
        return (-1);
    
    // Second pass: fill the new array with expanded arguments
    i = 0;
    int new_index = 0;
    
    while (g_data.args_array[i])
    {
        if (g_data.args_array[i] && wild_card_check(g_data.args_array[i]))
        {
            // Don't expand the first argument (command name)
            if (i == 0)
            {
                new_args[new_index] = dl_strdup(g_data.args_array[i]);
                new_index++;
            }
            else
            {
                char **expanded = expand_wildcard_pattern(g_data.args_array[i]);
                if (expanded)
                {
                    int j = 0;
                    while (expanded[j])
                    {
                        new_args[new_index] = dl_strdup(expanded[j]);
                        new_index++;
                        j++;
                    }
                    free_string_array(expanded);
                }
                else
                {
                    // No matches found, keep original pattern
                    new_args[new_index] = dl_strdup(g_data.args_array[i]);
                    new_index++;
                }
            }
        }
        else
        {
            // No wildcards, copy as-is
            new_args[new_index] = dl_strdup(g_data.args_array[i]);
            new_index++;
        }
        i++;
    }
    
    // Free old array and replace with new one
    free_string_array(g_data.args_array);
    g_data.args_array = new_args;
    g_data.args_count = expanded_count;
    
    return (0);
}

// Expand a single wildcard pattern into matching filenames
char **expand_wildcard_pattern(const char *pattern)
{
    if (!pattern || !wild_card_check((char *)pattern))
        return (NULL);
    
    DIR *dir;
    struct dirent *entry;
    char **matches = NULL;
    int match_count = 0;
    int max_matches = 100; // Reasonable limit
    char *dir_path = ".";
    char *file_pattern = (char *)pattern;
    int allocated_dir = 0;
    
    // Check if pattern contains a directory path
    char *last_slash = dl_strrchr(pattern, '/');
    if (last_slash)
    {
        // Extract directory path and file pattern
        int dir_len = last_slash - pattern;
        dir_path = dl_calloc(dir_len + 1, sizeof(char));
        if (!dir_path)
            return (NULL);
        dl_strncpy(dir_path, pattern, dir_len);
        dir_path[dir_len] = '\0';
        allocated_dir = 1;
        
        file_pattern = (char *)(last_slash + 1);
    }
    
    // Allocate initial matches array
    matches = dl_calloc(max_matches + 1, sizeof(char *));
    if (!matches)
    {
        if (allocated_dir)
            free(dir_path);
        return (NULL);
    }
    
    // Open the specified directory
    dir = opendir(dir_path);
    if (!dir)
    {
        free(matches);
        if (allocated_dir)
            free(dir_path);
        return (NULL);
    }
    
    // Read directory entries and find matches
    while ((entry = readdir(dir)) != NULL && match_count < max_matches)
    {
        // Skip . and .. entries
        if (dl_strcmp(entry->d_name, ".") == 0 || dl_strcmp(entry->d_name, "..") == 0)
            continue;
        
        // Check if filename matches pattern
        if (match_pattern(entry->d_name, file_pattern))
        {
            // If we're in a subdirectory, include the full path
            if (allocated_dir)
            {
                char *full_path = dl_strjoin(dl_strjoin(dir_path, "/"), entry->d_name);
                if (full_path)
                {
                    matches[match_count] = full_path;
                    match_count++;
                }
            }
            else
            {
                matches[match_count] = dl_strdup(entry->d_name);
                if (matches[match_count])
                    match_count++;
            }
        }
        

    }
    
    closedir(dir);
    
    // Clean up directory path
    if (allocated_dir)
        free(dir_path);
    
    // If no matches found, return NULL
    if (match_count == 0)
    {
        free(matches);
        return (NULL);
    }
    
    // Sort matches alphabetically
    sort_string_array(matches, match_count);
    
    return (matches);
}

// Sort a string array alphabetically
void sort_string_array(char **array, int count)
{
    if (!array || count <= 1)
        return;
    
    // Simple bubble sort for now
    for (int i = 0; i < count - 1; i++)
    {
        for (int j = 0; j < count - i - 1; j++)
        {
            if (dl_strcmp(array[j], array[j + 1]) > 0)
            {
                char *temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }
}

// Check if a filename matches a wildcard pattern
int match_pattern(const char *filename, const char *pattern)
{
    if (!filename || !pattern)
        return (0);
    
    // Simple * glob pattern matching
    if (dl_strchr(pattern, '*') != NULL)
    {
        // Handle * pattern (matches any sequence of characters)
        return match_glob_pattern(filename, pattern);
    }
    
    // Handle ? pattern (matches any single character)
    if (dl_strchr(pattern, '?') != NULL)
    {
        return match_question_pattern(filename, pattern);
    }
    
    // Handle [set] pattern (matches any character in the set)
    if (dl_strchr(pattern, '[') != NULL)
    {
        return match_bracket_pattern(filename, pattern);
    }
    
    // No wildcards, exact match
    return (dl_strcmp(filename, pattern) == 0);
}

// Match * glob pattern
int match_glob_pattern(const char *filename, const char *pattern)
{
    char *star_pos = dl_strchr(pattern, '*');
    if (!star_pos)
        return (0);
    
    // Split pattern into prefix and suffix
    int prefix_len = star_pos - pattern;
    int suffix_len = dl_strlen(star_pos + 1);
    

    
    // Check prefix
    if (prefix_len > 0 && dl_strncmp(filename, pattern, prefix_len) != 0)
        return (0);
    
    // Check suffix
    if (suffix_len > 0)
    {
        int filename_len = dl_strlen(filename);
        if (filename_len < prefix_len + suffix_len)
            return (0);
        

        
        if (dl_strncmp(filename + filename_len - suffix_len, star_pos + 1, suffix_len) != 0)
            return (0);
    }
    
    return (1);
}

// Match ? pattern (single character)
int match_question_pattern(const char *filename, const char *pattern)
{
    int filename_len = dl_strlen(filename);
    int pattern_len = dl_strlen(pattern);
    
    if (filename_len != pattern_len)
        return (0);
    
    for (int i = 0; i < pattern_len; i++)
    {
        if (pattern[i] != '?' && pattern[i] != filename[i])
            return (0);
    }
    
    return (1);
}

// Match [set] pattern
int match_bracket_pattern(const char *filename, const char *pattern)
{
    if (!filename || !pattern || pattern[0] != '[')
        return (0);
    
    int negated = 0;
    int i = 1;
    
    // Check for negation
    if (pattern[i] == '^')
    {
        negated = 1;
        i++;
    }
    
    // Find closing bracket
    while (pattern[i] && pattern[i] != ']')
        i++;
    
    if (pattern[i] != ']')
        return (0);  // Invalid pattern
    
    // Check if character is in the set
    int found = 0;
    for (int j = 1; (negated ? j < i : j < i); j++)
    {
        if (negated && pattern[j] == '^')
            continue;
        
        if (filename[0] == pattern[j])
        {
            found = 1;
            break;
        }
    }
    
    return (negated ? !found : found);
}
