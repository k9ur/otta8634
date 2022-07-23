// Sorts an array where each object entry (potentially) has some other entries which it must come before or after.
// Not particularly efficient.
// 
// A.before = [ "B" ] means that A must be before B in the array


let loop = true;
let trace = [];

function getObj(name, arr) {
	return arr.find(obj => obj.name === name);
}

// Manages dependencies by expanding and re-ordering the array
// If we have A.before = [ "B" ], B.before = [ "C" ], A.before will be expanded into [ "B", "C" ].
// depen is an array of the names of the (original or newly added) dependencies of obj
function manageDependencies(obj, depen, arr) {
	if(!obj.name || !depen.length || !loop) return loop;

	for(var name of depen) {
		if(name === obj.name) {
			trace.push(name);
			return loop = false;
		}

		let beforeObj = getObj(name, arr);

		let i1 = arr.indexOf(obj);
		let i2 = arr.indexOf(beforeObj);
		if(i1 > i2) {               // After the object which it needs to come before in the array
			arr.splice(i1, 1);      // Remove obj
			arr.splice(i2, 0, obj); // Insert obj right before beforeObj
		}

		let nextDepen = beforeObj.before.filter(n => !obj.before.includes(n)); // New dependencies
		if(!nextDepen.length) continue;

		obj.before = obj.before.concat(nextDepen); // Add new dependencies to the list
		let ret = manageDependencies(obj, nextDepen, arr); // Iterate again with new dependencies

		if(!ret) {
			trace.unshift(name);
			return ret;
		}
	}

	return true;
}

// Fixing befores then fixing afters isn't correct logic
// Function turns afters into befores, and also adds before to all objects if not previously present
// arr is required for getObj
function manageAftersAndBefores(obj, arr) {
	if(!obj.before) obj.before = [];
	if(!obj.name || !obj.after) return;

	for(var name of obj.after) {
		let beforeObj = getObj(name, arr);
		if(!beforeObj.before) beforeObj.before = [];
		if(beforeObj.before.includes(obj.name)) continue;

		beforeObj.before = beforeObj.before.concat(obj.name);
	}
	delete obj.after;
}

// Trace of how an object was linked back to itself as a dependency
function printTrace(trace) {
	let last = trace[trace.length - 1];
	trace.unshift(last);
	return last + " was linked back to itself as a dependency: " + trace.join(" -> ");
}


// Main function
// arr is the array to sort
function sortByDependencies(arr) {
	let res = [...arr]; // Copy array

	// Can't merge these iterations
	res.forEach(obj => manageAftersAndBefores(obj, res));
	for(var obj of res) {
		if(!manageDependencies(obj, obj.before, res)) break;
	}
	// Objects without a name can't be sorted, and by default are shuffled towards the end
	// Same behaviour for objects with no connected dependencies

	if(!loop) return printTrace(trace);
	return res;
}

module.exports = { sortByDependencies, manageDependencies, manageAftersAndBefores };
