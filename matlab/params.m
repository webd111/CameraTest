load('stereoParams.mat');
R = stereoParams.RotationOfCamera2;
R = reshape(R, 1, []);
T = [stereoParams.TranslationOfCamera2, zeros(1,6)];
camera1RadDist = [stereoParams.CameraParameters1.RadialDistortion, zeros(1,6)];
camera1TanDist = [stereoParams.CameraParameters1.TangentialDistortion, zeros(1,7)];
camera1FocalLength = [stereoParams.CameraParameters1.FocalLength, zeros(1,7)];
camera1PrincipalPoint = [stereoParams.CameraParameters1.PrincipalPoint, zeros(1,7)];
camera2RadDist = [stereoParams.CameraParameters2.RadialDistortion, zeros(1,6)];
camera2TanDist = [stereoParams.CameraParameters2.TangentialDistortion, zeros(1,7)];
camera2FocalLength = [stereoParams.CameraParameters2.FocalLength, zeros(1,7)];
camera2PrincipalPoint = [stereoParams.CameraParameters2.PrincipalPoint, zeros(1,7)];

name = {'R', 'T', 'camera1RadDist', 'camera1TanDist', 'cameralFocalLength', 'camera1PrincipalPoint'...
    'camera2RadDist', 'camera2TanDist', 'camera2FocalLength', 'camera2PrincipalPoint'};
Table = table(R' ,T', camera1RadDist', camera1TanDist', camera1FocalLength', camera1PrincipalPoint',...
    camera2RadDist', camera2TanDist', camera2FocalLength', camera2PrincipalPoint');
% Table = table(R ,T, camera1RadDist, camera1TanDist, camera1FocalLength, camera1PrincipalPoint,...
%     camera2RadDist, camera2TanDist, camera2FocalLength, camera2PrincipalPoint);
Table.Properties.VariableNames = name;

writetable(Table, 'stereoParams.csv', 'WriteRowNames', true, 'Delimiter', ',');