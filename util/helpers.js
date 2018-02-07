function isArray(x) {
    return x !== null && x !== undefined && x.constructor === Array;
}

function deconstructCellIndex(index, width) {
    return {
        x: index % width,
        y: Math.floor(index / width)
    };
}

function cellIndex(x, y, width) {
    return width * y + x;
}

module.exports = {
	isArray,
	deconstructCellIndex,
	cellIndex
};