// Sorts an array where each component (potentially) has some other values which it must come before or after.
// Not particularly efficient.
// 
// A.before = [ "B" ] means that A must be before B in the array


function getObj(name, list) {
	return list.find(obj => obj.name === name);
}

let loop = true;
// If we have A.before = [ "B" ], B.before = [ "C" ], A.before will be expanded into [ "B", "C" ].
function expandDependencies(obj, depen, list) {
	if(!depen || !loop) return;
	if(depen === []) return;

	for(var name of depen) {
		if(name === obj.name) {
			console.log(`${name} was linked back to itself as a dependency. Quitting.`); // TODO: find path
			loop = false;
		} else {
			_nextDepen = getObj(name, list).before;
			if(!_nextDepen) continue;

			nextDepen = _nextDepen.filter(n => !obj.before.includes(n));
			obj.before = obj.before.concat(nextDepen);
			expandDependencies(obj, nextDepen, list); // Iterate again with new dependencies
		}
	}
}

// Fixing befores then fixing afters isn't correct logic
function afterIntoBefore(obj, list) {
	if(!obj.name || !obj.after) return;
	for(var name of obj.after) {
		let beforeList = getObj(name, list).before;
		if(beforeList.includes(obj.name)) return;

		getObj(name, list).before = beforeList.concat(obj.name);
	}
	delete obj.after;
}


// Main function
function sortByDependencies(list) {
	for(var obj of list) {
		if(!obj.before) obj.before = [];
	}
	list.forEach(obj => afterIntoBefore(obj, list));
	list.forEach(obj => expandDependencies(obj, obj.before, list));
	if(!loop) return;

	let res = [...list];

	// Loop through every object
	for(var obj of res) {
		// Loop through each dependency of the object
		for(var beforeName of obj.before) {
			let beforeObj = getObj(beforeName, res);
			if(res.indexOf(obj) > res.indexOf(beforeObj)) { // After the obj which it needs to come before in the array
				res.splice(res.indexOf(obj), 1);            // Remove obj
				res.splice(res.indexOf(beforeObj), 0, obj); // Insert obj right before beforeObj
			}
		}
	}
	return res;
}

module.exports = { sortByDependencies, expandDependencies };
