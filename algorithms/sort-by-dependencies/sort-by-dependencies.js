// Sorts an array where each component (potentially) has some other values which it must come before or after.
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
function manageDependencies(obj, depen, arr) {
	if(!obj.name || !depen || !loop) return true;
	if(!depen.length) return true;

	for(var name of depen) {
		if(name === obj.name) {
			trace.unshift(name);
			return loop = false;
		} else {
			let beforeObj = getObj(name, arr);
			let nextDepen = beforeObj.before.filter(n => !obj.before.includes(n));
			if(!nextDepen.length) continue;

			obj.before = obj.before.concat(nextDepen);
			let ret = manageDependencies(obj, nextDepen, arr); // Iterate again with new dependencies

			if(!ret) {
				trace.unshift(name);
				return ret;
			} else if(arr.indexOf(obj) > arr.indexOf(beforeObj)) { // After the obj which it needs to come before in the array
				arr.splice(arr.indexOf(obj), 1);                   // Remove obj
				arr.splice(arr.indexOf(beforeObj), 0, obj);        // Insert obj right before beforeObj
			}
		}
	}

	return true;
}

// Fixing befores then fixing afters isn't correct logic
function afterIntoBefore(obj, arr) {
	if(!obj.name || !obj.after) return;
	for(var name of obj.after) {
		let beforeList = getObj(name, arr).before;
		if(beforeList.includes(obj.name)) return;

		getObj(name, arr).before = beforeList.concat(obj.name);
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
function sortByDependencies(arr) {
	let res = [...arr]; // Copy arr

	// Can't merge these iterations
	res.forEach(obj => {
		if(!obj.before) obj.before = [];
	});
	res.forEach(obj => afterIntoBefore(obj, res));
	for(var obj of res) {
		if(!manageDependencies(obj, obj.before, res)) break;
	}
	// Objects without a name can't be sorted, and by default are shuffled towards the end

	if(!loop) return printTrace(trace);
	return res;
}

module.exports = { sortByDependencies, manageDependencies };
