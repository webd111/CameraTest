% 标定板生成

length = 120;
I = zeros(1080, 1920, 'uint8');

for i = 1:1920
    for j = 1:1080
        if xor((mod(fix((i-1)/length),2)==0), (mod(fix((j-1)/length),2)==0))
            I(j, i) = 255;
        end
    end
end

imshow(I);
imwrite(I, 'CalibTarget.jpg');