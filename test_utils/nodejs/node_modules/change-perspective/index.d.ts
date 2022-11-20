export declare type QuadPoints = [number, number, number, number, number, number, number, number];
export declare type SinglePoint = [number, number];
export default function fixPerspective(srcPts: QuadPoints, dstPts: QuadPoints): (x: number, y: number) => [number, number];
