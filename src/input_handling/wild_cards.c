#include "cshell.h"
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>

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
    
    // Expand range patterns first (e.g., [a-c] -> [abc])
    char *expanded_pattern = expand_range_pattern(pattern);
    if (!expanded_pattern)
        return (NULL);
    
    DIR *dir;
    struct dirent *entry;
    char **matches = NULL;
    int match_count = 0;
    int max_matches = 100; // Reasonable limit
    char *dir_path = ".";
    char *file_pattern = expanded_pattern;
    int allocated_dir = 0;
    
    // Check if pattern contains a directory path
    char *last_slash = dl_strrchr(expanded_pattern, '/');
    if (last_slash)
    {
        // Extract directory path and file pattern
        int dir_len = last_slash - expanded_pattern;
        dir_path = dl_calloc(dir_len + 1, sizeof(char));
        if (!dir_path)
        {
            free(expanded_pattern);
            return (NULL);
        }
        dl_strncpy(dir_path, expanded_pattern, dir_len);
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
        free(expanded_pattern);
        return (NULL);
    }
    
    // Open the specified directory
    dir = opendir(dir_path);
    if (!dir)
    {
        free(matches);
        if (allocated_dir)
            free(dir_path);
        free(expanded_pattern);
        return (NULL);
    }
    
    // Read directory entries and find matches
    while ((entry = readdir(dir)) != NULL && match_count < max_matches)
    {
        // Skip . and .. entries
        if (dl_strcmp(entry->d_name, ".") == 0 || dl_strcmp(entry->d_name, "..") == 0)
            continue;
        
        // Skip subdirectories - only check files for wildcard patterns
        struct stat st;
        char full_path[PATH_MAX];
        if (allocated_dir)
        {
            // Construct full path: dir_path + "/" + entry->d_name
            int dir_len = dl_strlen(dir_path);
            int name_len = dl_strlen(entry->d_name);
            
            if (dir_len + 1 + name_len < PATH_MAX)
            {
                dl_strcpy(full_path, dir_path);
                full_path[dir_len] = '/';
                full_path[dir_len + 1] = '\0';
                dl_strcpy(full_path + dir_len + 1, entry->d_name);
            }
            else
            {
                full_path[0] = '\0';  // Path too long
            }
        }
        else
            dl_strcpy(full_path, entry->d_name);
        
        if (stat(full_path, &st) == 0 && S_ISDIR(st.st_mode))
            continue;  // Skip directories
        
        // Check if filename matches pattern
        if (match_pattern(entry->d_name, file_pattern))
        {
            // If we're in a subdirectory, include the full path
            if (allocated_dir)
            {
                char *full_path_result = dl_strjoin(dl_strjoin(dir_path, "/"), entry->d_name);
                if (full_path_result)
                {
                    matches[match_count] = full_path_result;
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
    
    // Clean up directory path and expanded pattern
    if (allocated_dir)
        free(dir_path);
    free(expanded_pattern);
    
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

// Expand range patterns like [a-c] to [abc]
char *expand_range_pattern(const char *pattern)
{
    if (!pattern || !dl_strchr(pattern, '-'))
        return (dl_strdup(pattern));
    
    char *expanded = dl_calloc(dl_strlen(pattern) * 3, sizeof(char)); // Allow for expansion
    if (!expanded)
        return (NULL);
    
    int i = 0, j = 0;
    while (pattern[i])
    {
        if (pattern[i] == '[')
        {
            expanded[j++] = pattern[i++];
            
            // Look for range pattern
            while (pattern[i] && pattern[i] != ']')
            {
                if (pattern[i] == '-' && i > 0 && pattern[i + 1] && pattern[i + 1] != ']')
                {
                    // Expand range
                    char start = pattern[i - 1];
                    char end = pattern[i + 1];
                    
                    if (start < end)
                    {
                        for (char c = start; c <= end; c++)
                        {
                            expanded[j++] = c;
                        }
                    }
                    else
                    {
                        // Invalid range, keep as-is
                        expanded[j++] = pattern[i - 1];
                        expanded[j++] = pattern[i];
                        expanded[j++] = pattern[i + 1];
                    }
                    i += 2; // Skip the range
                }
                else
                {
                    expanded[j++] = pattern[i++];
                }
            }
            
            if (pattern[i] == ']')
                expanded[j++] = pattern[i++];
        }
        else
        {
            expanded[j++] = pattern[i++];
        }
    }
    expanded[j] = '\0';
    
    return (expanded);
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
    
    // Check for multiple wildcard types
    int has_star = (dl_strchr(pattern, '*') != NULL);
    int has_question = (dl_strchr(pattern, '?') != NULL);
    int has_bracket = (dl_strchr(pattern, '[') != NULL);
    
    // Special case: if we have brackets but no other complex wildcards, use bracket matcher
    if (has_bracket && !has_star && !has_question)
    {
        return match_bracket_pattern(filename, pattern);
    }
    
    // If we have multiple wildcard types, we need more sophisticated matching
    if ((has_star && has_bracket) || (has_star && has_question) || (has_bracket && has_question))
    {
        return match_mixed_pattern(filename, pattern);
    }
    
    // Simple * glob pattern matching
    if (has_star)
    {
        return match_glob_pattern(filename, pattern);
    }
    
    // Handle ? pattern (single character)
    if (has_question)
    {
        return match_question_pattern(filename, pattern);
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
        if (pattern[i] == '?')
            continue;  // ? matches any character
        else if (pattern[i] == '[')
        {
            // Handle bracket pattern
            int j = i + 1;
            int negated = 0;
            
            // Check for negation
            if (pattern[j] == '^')
            {
                negated = 1;
                j++;
            }
            
            // Find closing bracket
            while (pattern[j] && pattern[j] != ']')
                j++;
            
            if (pattern[j] != ']')
                return (0);  // Invalid pattern
            
            // Check if character is in the set
            int found = 0;
            for (int k = i + 1; k < j; k++)
            {
                if (negated && pattern[k] == '^')
                    continue;
                
                if (filename[i] == pattern[k])
                {
                    found = 1;
                    break;
                }
            }
            
            if (negated ? found : !found)
                return (0);  // Character not in set (or in negated set)
            
            i = j;  // Skip to after the bracket pattern
        }
        else if (pattern[i] != filename[i])
            return (0);
    }
    
    return (1);
}

// Match [set] pattern (character set)
int match_bracket_pattern(const char *filename, const char *pattern)
{
    int filename_len = dl_strlen(filename);
    int pattern_len = dl_strlen(pattern);
    
    // For bracket patterns, we need to handle the pattern character by character
    int filename_pos = 0;
    int pattern_pos = 0;
    
    while (pattern_pos < pattern_len && filename_pos < filename_len)
    {
        if (pattern[pattern_pos] == '[')
        {
            // Handle bracket pattern
            int j = pattern_pos + 1;
            int negated = 0;
            
            // Check for negation
            if (pattern[j] == '^')
            {
                negated = 1;
                j++;
            }
            
            // Find closing bracket
            while (j < pattern_len && pattern[j] != ']')
                j++;
            
            if (j >= pattern_len || pattern[j] != ']')
                return (0);  // Invalid pattern
            
            // Check if character is in the set
            int found = 0;
            for (int k = pattern_pos + 1; k < j; k++)
            {
                if (negated && pattern[k] == '^')
                    continue;
                
                if (filename[filename_pos] == pattern[k])
                {
                    found = 1;
                    break;
                }
            }
            
            if (negated ? found : !found)
                return (0);  // Character not in set (or in negated set)
            
            pattern_pos = j + 1;  // Skip to after the bracket pattern
            filename_pos++;        // Move to next character in filename
        }
        else if (pattern[pattern_pos] == '?')
        {
            // ? matches any single character
            pattern_pos++;
            filename_pos++;
        }
        else if (pattern[pattern_pos] == '*')
        {
            // * matches any sequence of characters
            // For bracket patterns with *, we need to handle this specially
            if (pattern_pos + 1 >= pattern_len)
            {
                // * at the end, match everything
                return (1);
            }
            else
            {
                // Try to match the remaining pattern after *
                for (int i = 0; filename_pos + i <= filename_len; i++)
                {
                    if (match_bracket_pattern(filename + filename_pos + i, pattern + pattern_pos + 1))
                        return (1);
                }
                return (0);
            }
        }
        else if (pattern[pattern_pos] == filename[filename_pos])
        {
            // Exact character match
            pattern_pos++;
            filename_pos++;
        }
        else
        {
            return (0);  // No match
        }
    }
    
    // Both pattern and filename must be fully consumed
    return (pattern_pos >= pattern_len && filename_pos >= filename_len);
}

// Match mixed wildcard patterns (e.g., [abc]*.txt, test?*.c)
int match_mixed_pattern(const char *filename, const char *pattern)
{
    if (!filename || !pattern)
        return (0);
    
    // For mixed patterns, we'll use a more flexible approach
    // Split the pattern into segments and match each segment
    
    char *expanded_pattern = expand_range_pattern(pattern);
    if (!expanded_pattern)
        return (0);
    
    int result = match_mixed_pattern_recursive(filename, expanded_pattern, 0, 0);
    
    free(expanded_pattern);
    
    return result;
}

// Recursive helper for mixed pattern matching
int match_mixed_pattern_recursive(const char *filename, const char *pattern, int filename_pos, int pattern_pos)
{
    int filename_len = dl_strlen(filename);
    int pattern_len = dl_strlen(pattern);
    
    // If we've consumed both pattern and filename, it's a match
    if (pattern_pos >= pattern_len)
        return (filename_pos >= filename_len);
    
    // If we've consumed the filename but not the pattern, it's not a match
    if (filename_pos >= filename_len)
        return (0);
    
    char current_char = pattern[pattern_pos];
    
    if (current_char == '*')
    {
        // * matches any sequence of characters
        // Try matching 0 or more characters
        for (int i = 0; filename_pos + i <= filename_len; i++)
        {
            if (match_mixed_pattern_recursive(filename, pattern, filename_pos + i, pattern_pos + 1))
                return (1);
        }
        return (0);
    }
    else if (current_char == '?')
    {
        // ? matches any single character
        return match_mixed_pattern_recursive(filename, pattern, filename_pos + 1, pattern_pos + 1);
    }
    else if (current_char == '[')
    {
        // Handle bracket pattern
        int j = pattern_pos + 1;
        int negated = 0;
        
        // Check for negation
        if (pattern[j] == '^')
        {
            negated = 1;
            j++;
        }
        
        // Find closing bracket
        while (j < pattern_len && pattern[j] != ']')
            j++;
        
        if (j >= pattern_len || pattern[j] != ']')
            return (0);  // Invalid pattern
        
        // Check if character is in the set
        int found = 0;
        for (int k = pattern_pos + 1; k < j; k++)
        {
            if (negated && pattern[k] == '^')
                continue;
            
            if (filename[filename_pos] == pattern[k])
            {
                found = 1;
                break;
            }
        }
        
        if (negated ? found : !found)
            return (0);  // Character not in set (or in negated set)
        
        return match_mixed_pattern_recursive(filename, pattern, filename_pos + 1, j + 1);
    }
    else if (current_char == filename[filename_pos])
    {
        // Exact character match
        return match_mixed_pattern_recursive(filename, pattern, filename_pos + 1, pattern_pos + 1);
    }
    else
    {
        return (0);  // No match
    }
}


