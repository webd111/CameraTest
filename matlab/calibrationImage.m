pathOutput = 'C:\Users\12257\Desktop\GraduationThesis\QtProject\dataset\calibration';
pathInput = 'C:\Users\12257\Documents\ZED';

mkdir([pathOutput,'\left']);
mkdir([pathOutput,'\right']);

files = dir(fullfile(pathInput, '*.png'));

for i = 1:size(files, 1)
    img = imread([pathInput, '\', files(i).name]);
    imgL = img(1:size(img,1), 1:size(img,2)/2);
    imgR = img(1:size(img,1), size(img,2)/2+1:size(img,2));
    imwrite(imgL, [pathOutput, '\left', '\', num2str(i), '.png'], 'png');
    imwrite(imgR, [pathOutput, '\right', '\', num2str(i), '.png'], 'png');
end