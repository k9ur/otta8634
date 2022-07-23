# Sorts a list where each dict entry (potentially) has some other entries which it must come before or after.
# Not particularly efficient.
# 
# A["before"] = [ "B" ] means that A must be before B in the list


loop = True
trace = []

def get_dct(name, lst):
	return next(dct for dct in lst if "name" in dct.keys() and dct["name"] == name)

# Manages dependencies by expanding and re-ordering the list
# If we have A["before"] = [ "B" ], B["before"] = [ "C" ], A["before"] will be expanded into [ "B", "C" ].
# depen is a list of the names of the (original or newly added) dependencies of dct
def manage_dependencies(dct, depen, lst):
	global loop
	if "name" not in dct.keys() or not len(depen) or not loop:
		return loop

	for name in depen:
		if name == dct["name"]:
			trace.append(name)
			loop = False
			return False
		
		before_dct = get_dct(name, lst)

		i1 = lst.index(dct)
		i2 = lst.index(before_dct)
		if i1 > i2:                     # After the dict which it needs to come before in the list
			lst.insert(i2, lst.pop(i1)) # Move dct to right before before_dct

		next_depen = list(set(before_dct["before"]) - set(dct["before"])) # New dependencies
		if not len(next_depen):
			continue

		dct["before"] += next_depen # Add new dependencies to the list
		ret = manage_dependencies(dct, next_depen, lst) # Iterate again with new dependencies

		if not ret:
			trace.insert(0, name)
			return ret
	
	return True

# Fixing befores then fixing afters isn't correct logic
# Function turns afters into befores, and also adds before to all dicts if not previously present
# lst is required for get_dct
def manage_afters_and_befores(dct, lst):
	keys = dct.keys()
	if "before" not in keys:
		dct["before"] = []
	if "name" not in keys or "after" not in keys:
		return

	for name in dct["after"]:
		before_dct = get_dct(name, lst)
		if "before" not in before_dct.keys():
			before_dct["before"] = []
		if dct["name"] in before_dct["before"]:
			continue

		before_dct["before"].append(dct["name"])
	del dct["after"]

# Trace of how an dict was linked back to itself as a dependency
def print_trace(trace):
	last = trace[len(trace) - 1]
	trace.insert(0, last)
	return str(last) + " was linked back to itself as a dependency: " + " -> ".join(map(str, trace))


# Main function
# lst is the list to sort
def sort_by_dependencies(lst):
	global loop
	res = lst.copy() # Copy list

	# Can't merge these iterations
	for dct in res:
		manage_afters_and_befores(dct, lst)
	for dct in res:
		if not manage_dependencies(dct, dct["before"], res):
			break
	# Dicts without a name can't be sorted, and by default are shuffled towards the end
	# Same behaviour for dicts with no connected dependencies

	if not loop:
		return print_trace(trace)
	return res
