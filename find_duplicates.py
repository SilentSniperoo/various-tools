# Duplicate file finder.
# Usage: python find_duplicates.py <folder> (more folders...)

import os
import sys
import hashlib
from collections import defaultdict
from difflib import SequenceMatcher
from functools import partial
from concurrent.futures import ProcessPoolExecutor

# Stores file contents and tracks how recently they have been used
class FileContentCache:
    def __init__(self):
        self.storage = {}
        self.oldest_id = 0
        self.next_id = 0

    # Move forward one id
    def get_next_id(self):
        id = self.next_id
        self.next_id += 1
        return id

    # Cache an object and return its cache id
    def add(self, data):
        id = self.get_next_id()
        if not len(self.storage):
            self.oldest_id = id
        self.storage[id] = data
        return id

    # Prolong the life of a cached object and retrieve its data and new id
    def get(self, id):
        if not id in self.storage:
            return False
        data = self.storage.pop(id)
        id = self.get_next_id()
        self.storage[id] = data
        return (id, data)

    # End the life of the oldest cached object
    def drop_oldest(self):
        if len(self.storage):
            while not self.oldest_id in self.storage:
                self.oldest_id += 1
            self.storage.pop(self.oldest_id)

# Cached copy of all relevant file attributes and contents for comparing
class FileDetails:
    # Keep track of which files have their contents cached
    file_contents_cache = FileContentCache()

    def __init__(self, file_path, file_name):
        self.file_path = file_path
        self.file_name = file_name
        self.file_size = os.path.getsize(file_path)
        self.file_contents_cache_id = -1
        self.file_has_been_hashed = False

    def get_file_contents(self):
        id_and_data = FileDetails.file_contents_cache.get(self.file_contents_cache_id)
        if (id_and_data):
            print("get_file_contents: if -> %d" % id_and_data[0])
            self.file_contents_cache_id = id_and_data[0]
            return id_and_data[1]
        while True:
            try:
                print("get_file_contents: try")
                file_object = open(self.file_path, "rb")
                file_contents = file_object.read(self.file_size)
                print("get_file_contents: cache")
                self.file_contents_cache_id = FileDetails.file_contents_cache.add(file_contents)
                return file_contents
            except:
                print("get_file_contents: drop")
                FileDetails.file_contents_cache.drop_oldest()

    def get_file_hash(self):
        # Hash both files if they haven't been hashed yet
        if not self.file_has_been_hashed:
            file_hasher = hashlib.sha1()
            file_hasher.update(self.get_file_contents())
            self.file_hash = file_hasher.digest()
        return self.file_hash

    def get_similarity_ratio(self, other):
        # Collect the test results and compile them into a unified metric
        # All tests should be in the range [0,1]
        tests = []
        # Compare file names
        tests.append(SequenceMatcher(self.file_name, other.file_name).ratio())
        # Compare file sizes
        tests.append(other.file_size / self.file_size if self.file_size > other.file_size else self.file_size / other.file_size)
        # Compare hash of file contents
        #tests.append(1.0 if self.get_file_hash() == other.get_file_hash() else 0.0)
        # Compare file contents
        #tests.append(SequenceMatcher(self.get_file_contents(), other.get_file_contents()).ratio())
        # Return the average of the results with a bonus for each exact match
        return sum(tests) / len(tests) + tests.count(1.0)

# Measurement of similarity between two files
class FileComparison:
    def __str__(self):
        return "Similarity[%f] for [%s] and [%s]" % (self.similarity_ratio, self.file_path_a, self.file_path_b)

    def __init__(self, file_a, file_b):
        self.similarity_ratio = file_a.get_similarity_ratio(file_b)
        self.file_path_a = file_a.file_path
        self.file_path_b = file_b.file_path

# Takes an iterable of directory paths and returns a list of FileDetails
def get_files_in(root_directories):
    details_list = []
    for root in root_directories:
        # Recurse through each given root directory and collect files
        for directory_path, _contained_directory_names, contained_file_names in os.walk(root):
            for file_name in contained_file_names:
                file_path = os.path.join(directory_path, file_name)
                try:
                    # If a path to a soft symlink, dereference to the real file
                    file_path = os.path.realpath(file_path)
                    # Get the details of the file in a bundle
                    file_details = FileDetails(file_path, file_name)
                except OSError:
                    # Could not access the file because of permissions, etc.
                    continue
                # Store the attributes of the file and the file itself
                details_list.append(file_details)
    return details_list

def compare_file_details(pair_of_files):
    pass

# Takes an iterable of directory paths and returns the most similar pairs
def get_most_similar_files_in(root_directories, max_result_count):
    result_list = []
    details_list = get_files_in(root_directories)

    group_size = 500

    # Compare all items in each group with the other items in the same group
    for group_index in range(0, len(details_list), group_size):
        print("Comparing group %d internally" % (group_index / group_size))
        # Very first file has no previous to compare to, so skip it
        for i1 in range(group_index + 1, min(group_index + group_size, len(details_list))):
            # Each file's details should be compared with the previous ones
            for i2 in range(group_index, i1):
                result_list.append(FileComparison(details_list[i1], details_list[i2]))

    print("All groups compared internally")

    # Compare all items from each group with all the items from each other group
    for group_index1 in range(group_size, len(details_list), group_size):
        for group_index2 in range(0, group_index1, group_size):
            print("Comparing group %d to group %d" % (group_index1 / group_size, group_index2 / group_size))
            for i1 in range(group_index1, min(group_index1 + group_size, len(details_list))):
                for i2 in range(group_index2, group_index2 + group_size):
                    result_list.append(FileComparison(details_list[i1], details_list[i2]))


    #direction = 1
    ## Very first file has no previous to compare to, so skip it
    #for first_index in range(1, len(details_list) - 1):
    #    # Each file's details should be compared with the remaining ones
    #    for second_index in range(0, first_index - 1, direction):
    #        result_list.append(FileComparison(details_list[first_index], details_list[second_index]))
    #    # Reversing the direction puts the most recently touched files next
    #    direction = -direction

    sorted(result_list, lambda details_object: details_object.similarity_ratio, True)
    return result_list if len(result_list) <= max_result_count else result_list[:max_result_count]

# If run directly from the terminal
if __name__ == "__main__":
    # Make sure we have directories to operate on
    if sys.argv[1:]:
        most_similar_files = get_most_similar_files_in(sys.argv[1:], 100)
        for comparison in most_similar_files:
            print(comparison)
    else:
        print("Usage: python %s <folder> (more folders...)" % sys.argv[0])