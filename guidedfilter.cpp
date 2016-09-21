// Guided Filter.cpp : 定义控制台应用程序的入口点。
//
#include"stdafx.h"
#include"guidedfilter.h"


cv::Mat getimage(cv::Mat &a)
{
	//This function is not used
	int hei = a.rows;
	int wid = a.cols;
	cv::Mat I(hei, wid, CV_64FC1);
	//convert image depth to CV_64F
	a.convertTo(I, CV_64FC1, 1.0 / 255.0);
	//normalize the pixel to 0~1
	/*
	for( int i = 0; i< hei; i++){
	double *p = I.ptr<double>(i);
	for( int j = 0; j< wid; j++){
	p[j] = p[j]/255.0;
	}
	}
	*/
	return I;
}

cv::Mat cumsum(cv::Mat &imSrc, int rc)
{
	//This function compute the cumulative sum of a matrix
	if (!imSrc.data)
	{
		std::cout << "no data input!\n" << std::endl;
	}
	int hei = imSrc.rows;
	int wid = imSrc.cols;
	cv::Mat imCum = imSrc.clone();
	if (rc == 1)
	{
		for (int i = 1; i < hei; i++)
		{
			for (int j = 0; j< wid; j++)
			{
				imCum.at<double>(i, j) += imCum.at<double>(i - 1, j);
			}
		}
	}

	if (rc == 2)
	{
		for (int i = 0; i < hei; i++)
		{
			for (int j = 1; j< wid; j++)
			{
				imCum.at<double>(i, j) += imCum.at<double>(i, j - 1);
			}
		}
	}
	return imCum;
}

cv::Mat boxfilter(cv::Mat imSrc, int r)
{
	//Box filter compute the sum of the window in an image, refer to http://blog.csdn.net/lanbing510/article/details/28696833 for more details
	//This code is write with reference to published Matlab code
	int hei = imSrc.rows;
	int wid = imSrc.cols;
	cv::Mat imDst = cv::Mat::zeros(hei, wid, CV_64FC1);
	//imCum = cumsum(imSrc, 1);
	cv::Mat imCum = cumsum(imSrc, 1);
	//imDst(1:r+1, :) = imCum(1+r:2*r+1, :);
	for (int i = 0; i<r + 1; i++)
	{
		for (int j = 0; j<wid; j++)
		{
			imDst.at<double>(i, j) = imCum.at<double>(i + r, j);
		}
	}
	//imDst(r+2:hei-r, :) = imCum(2*r+2:hei, :) - imCum(1:hei-2*r-1, :);
	for (int i = r + 1; i<hei - r; i++)
	{
		for (int j = 0; j<wid; j++)
		{
			imDst.at<double>(i, j) = imCum.at<double>(i + r, j) - imCum.at<double>(i - r - 1, j);
		}
	}
	//imDst(hei-r+1:hei, :) = repmat(imCum(hei, :), [r, 1]) - imCum(hei-2*r:hei-r-1, :);
	for (int i = hei - r; i< hei; i++)
	{
		for (int j = 0; j< wid; j++)
		{
			imDst.at<double>(i, j) = imCum.at<double>(hei - 1, j) - imCum.at<double>(i - r - 1, j);
		}
	}
	imCum.release();
	imCum = cumsum(imDst, 2);
	//imDst(:, 1:r+1) = imCum(:, 1+r:2*r+1);
	for (int i = 0; i<hei; i++)
	{
		for (int j = 0; j<r + 1; j++)
		{
			imDst.at<double>(i, j) = imCum.at<double>(i, j + r);
		}
	}
	//imDst(:, r+2:wid-r) = imCum(:, 2*r+2:wid) - imCum(:, 1:wid-2*r-1);
	for (int i = 0; i<hei; i++)
	{
		for (int j = r + 1; j<wid - r; j++)
		{
			imDst.at<double>(i, j) = imCum.at<double>(i, j + r) - imCum.at<double>(i, j - r - 1);
		}
	}
	//imDst(:, wid-r+1:wid) = repmat(imCum(:, wid), [1, r]) - imCum(:, wid-2*r:wid-r-1);
	for (int i = 0; i< hei; i++)
	{
		for (int j = wid - r; j<wid; j++)
		{
			imDst.at<double>(i, j) = imCum.at<double>(i, wid - 1) - imCum.at<double>(i, j - r - 1);
		}
	}
	return imDst;
}

cv::Mat guidedfilter(cv::Mat I, cv::Mat p, int r, double eps)
{
	//Please refer to the paper at http://research.microsoft.com/en-us/um/people/kahe/eccv10/ for more details of guided filter 
	// This code is modified from published Matlab code, refer to http://blog.csdn.net/wangyaninglm/article/details/44838545

	/*
	% GUIDEDFILTER   O(1) time implementation of guided filter.
	%
	%   - guidance image: I (should be a gray-scale/single channel image)
	%   - filtering input image: p (should be a gray-scale/single channel image)
	%   - local window radius: r
	%   - regularization parameter: eps
	*/

	cv::Mat _I;
	I.convertTo(_I, CV_64FC1);
	// I = _I;

	cv::Mat _p;
	p.convertTo(_p, CV_64FC1);
	p = _p;

	//[hei, wid] = size(I);
	int hei = _I.rows;
	int wid = _I.cols;

	//N = boxfilter(ones(hei, wid), r); % the size of each local patch; N=(2r+1)^2 except for boundary pixels.
	cv::Mat N;
	N = boxfilter(cv::Mat::ones(hei, wid, _I.type()), r);

	//mean_I = boxfilter(I, r) ./ N;
	cv::Mat mean_I = boxfilter(_I, r);
	divide(mean_I, N, mean_I);

	//mean_p = boxfilter(p, r) ./ N;
	cv::Mat mean_p = boxfilter(p, r);
	divide(mean_p, N, mean_p);

	//mean_Ip = boxfilter(I.*p, r) ./ N;
	cv::Mat mean_Ip = boxfilter(_I.mul(p), r);
	divide(mean_Ip, N, mean_Ip);

	//cov_Ip = mean_Ip - mean_I .* mean_p; % this is the covariance of (I, p) in each local patch.
	cv::Mat cov_Ip = mean_Ip - mean_I.mul(mean_p);
	mean_Ip.release();

	//mean_II = boxfilter(I.*I, r) ./ N;
	cv::Mat mean_II = boxfilter(_I.mul(_I), r);
	divide(mean_II, N, mean_II);


	//var_I = mean_II - mean_I .* mean_I;
	cv::Mat var_I = mean_II - mean_I.mul(mean_I);
	mean_II.release();

	//a = cov_Ip ./ (var_I + eps); % Eqn. (5) in the paper;
	cv::Mat a = cov_Ip / (var_I + eps);
	cov_Ip.release();
	var_I.release();

	//b = mean_p - a .* mean_I; % Eqn. (6) in the paper;
	cv::Mat b = mean_p - a.mul(mean_I);
	mean_I.release();
	mean_p.release();

	//mean_a = boxfilter(a, r) ./ N;
	cv::Mat mean_a = boxfilter(a, r);
	a.release();
	divide(mean_a, N, mean_a);


	//mean_b = boxfilter(b, r) ./ N;
	cv::Mat mean_b = boxfilter(b, r);
	b.release();
	divide(mean_b, N, mean_b);

	//q = mean_a .* I + mean_b; % Eqn. (8) in the paper;
	cv::Mat q = mean_a.mul(_I) + mean_b;

	return q;
}

cv::Mat guidedfilter_color(cv::Mat I, cv::Mat p, int r, double eps)
{
	//This function is not used, there may be bugs in the code
	//Please don't use it
	/*
	% GUIDEDFILTER for color image as guide image  O(1) time implementation of guided filter.
	%
	%   - guidance image: I (should be a gray-scale/single channel image)
	%   - filtering input image: p (should be a gray-scale/single channel image)
	%   - local window radius: r
	%   - regularization parameter: eps
	*/
	cv::Mat _I;
	I.convertTo(_I, CV_64FC1);

	cv::Mat _p;
	p.convertTo(_p, CV_64FC1);
	p = _p;

	std::vector<cv::Mat> I_bgr(3);


	split(_I, I_bgr);

	//[hei, wid] = size(I);
	int hei = _I.rows;
	int wid = _I.cols;

	//N = boxfilter(ones(hei, wid), r); % the size of each local patch; N=(2r+1)^2 except for boundary pixels.
	cv::Mat N = boxfilter(cv::Mat::ones(hei, wid, I_bgr[0].type()), r);

	//mean_I_r = boxfilter(I(:, :, 1), r) ./ N;
	//mean_I_g = boxfilter(I(:, :, 2), r) ./ N;
	//mean_I_b = boxfilter(I(:, :, 3), r) ./ N;
	cv::Mat mean_I_b = boxfilter(I_bgr.at(0), r);
	divide(mean_I_b, N, mean_I_b);
	cv::Mat mean_I_g = boxfilter(I_bgr.at(1), r);
	divide(mean_I_g, N, mean_I_g);
	cv::Mat mean_I_r = boxfilter(I_bgr.at(2), r);
	divide(mean_I_r, N, mean_I_r);


	//mean_p = boxfilter(p, r) ./ N;
	cv::Mat mean_p = boxfilter(p, r);
	divide(mean_p, N, mean_p);
	//cv::boxFilter(p, mean_p, CV_64FC1, cv::Size(r, r));

	//mean_Ip_r = boxfilter(I(:, :, 1).*p, r) ./ N;
	//mean_Ip_g = boxfilter(I(:, :, 2).*p, r) ./ N;
	//mean_Ip_b = boxfilter(I(:, :, 3).*p, r) ./ N;

	cv::Mat mean_Ip_b = boxfilter(I_bgr.at(0).mul(p), r);
	divide(mean_Ip_b, N, mean_Ip_b);
	cv::Mat mean_Ip_g = boxfilter(I_bgr.at(1).mul(p), r);
	divide(mean_Ip_g, N, mean_Ip_g);
	cv::Mat mean_Ip_r = boxfilter(I_bgr.at(2).mul(p), r);
	divide(mean_Ip_r, N, mean_Ip_r);





	//% covariance of (I, p) in each local patch.
	//cov_Ip_r = mean_Ip_r - mean_I_r .* mean_p;
	//cov_Ip_g = mean_Ip_g - mean_I_g .* mean_p;
	//cov_Ip_b = mean_Ip_b - mean_I_b .* mean_p;

	cv::Mat cov_Ip_b = mean_Ip_b - mean_I_b.mul(mean_p);
	cv::Mat cov_Ip_g = mean_Ip_g - mean_I_g.mul(mean_p);
	cv::Mat cov_Ip_r = mean_Ip_r - mean_I_r.mul(mean_p);

	// variance of I in each local patch: the matrix Sigma in Eqn (14).
	// Note the variance in each local patch is a 3x3 symmetric matrix:
	/*           rr, rg, rb
	Sigma = rg, gg, gb
	rb, gb, bb
	var_I_rr = boxfilter(I(:, :, 1).*I(:, :, 1), r) ./ N - mean_I_r .*  mean_I_r;
	var_I_rg = boxfilter(I(:, :, 1).*I(:, :, 2), r) ./ N - mean_I_r .*  mean_I_g;
	var_I_rb = boxfilter(I(:, :, 1).*I(:, :, 3), r) ./ N - mean_I_r .*  mean_I_b;
	var_I_gg = boxfilter(I(:, :, 2).*I(:, :, 2), r) ./ N - mean_I_g .*  mean_I_g;
	var_I_gb = boxfilter(I(:, :, 2).*I(:, :, 3), r) ./ N - mean_I_g .*  mean_I_b;
	var_I_bb = boxfilter(I(:, :, 3).*I(:, :, 3), r) ./ N - mean_I_b .*  mean_I_b;
	*/
	cv::Mat var_I_bb;
	cv::Mat var_I_bg;
	cv::Mat var_I_br;
	cv::Mat var_I_gg;
	cv::Mat var_I_gr;
	cv::Mat var_I_rr;

	var_I_bb = boxfilter(I_bgr[0].mul(I_bgr[0]), r);
	var_I_bg = boxfilter(I_bgr[1].mul(I_bgr[0]), r);
	var_I_br = boxfilter(I_bgr[2].mul(I_bgr[0]), r);
	var_I_gg = boxfilter(I_bgr[1].mul(I_bgr[1]), r);
	var_I_gr = boxfilter(I_bgr[2].mul(I_bgr[1]), r);
	var_I_rr = boxfilter(I_bgr[2].mul(I_bgr[2]), r);

	divide(var_I_bb, N, var_I_bb);
	var_I_bb = var_I_bb - mean_I_b.mul(mean_I_b);
	divide(var_I_bg, N, var_I_bg);
	var_I_bb = var_I_bb - mean_I_b.mul(mean_I_g);
	divide(var_I_br, N, var_I_br);
	var_I_bb = var_I_bb - mean_I_b.mul(mean_I_r);
	divide(var_I_gg, N, var_I_gg);
	var_I_bb = var_I_bb - mean_I_g.mul(mean_I_g);
	divide(var_I_gr, N, var_I_gr);
	var_I_bb = var_I_bb - mean_I_g.mul(mean_I_r);
	divide(var_I_rr, N, var_I_rr);
	var_I_bb = var_I_bb - mean_I_r.mul(mean_I_r);




	/*
	a = zeros(hei, wid, 3);
	for y=1:hei
		for x=1:wid
			Sigma = [var_I_rr(y, x), var_I_rg(y, x), var_I_rb(y, x);
			var_I_rg(y, x), var_I_gg(y, x), var_I_gb(y, x);
			var_I_rb(y, x), var_I_gb(y, x), var_I_bb(y, x)];
			%Sigma = Sigma + eps * eye(3);

			cov_Ip = [cov_Ip_r(y, x), cov_Ip_g(y, x), cov_Ip_b(y, x)];

			a(y, x, :) = cov_Ip * inv(Sigma + eps * eye(3)); % Eqn. (14) in the paper;
		end
	end
	*/
	std::vector<cv::Mat> a(3, cv::Mat::zeros(hei, wid, CV_64FC1));
	cv::Mat sigma = cv::Mat::zeros(3, 3, CV_64FC1);
	cv::Mat cov_Ip = cv::Mat::zeros(1, 3, CV_64FC1);

	int x, y;
	for (x = 0; x<wid; x++)
	{
		for (y = 0; y<hei; y++)
		{
			sigma.at<double>(0, 0) = var_I_bb.at<double>(y, x);
			sigma.at<double>(0, 1) = var_I_bg.at<double>(y, x);
			sigma.at<double>(0, 2) = var_I_br.at<double>(y, x);
			sigma.at<double>(1, 0) = var_I_bg.at<double>(y, x);
			sigma.at<double>(1, 1) = var_I_gg.at<double>(y, x);
			sigma.at<double>(1, 2) = var_I_gr.at<double>(y, x);
			sigma.at<double>(2, 0) = var_I_br.at<double>(y, x);
			sigma.at<double>(2, 1) = var_I_gr.at<double>(y, x);
			sigma.at<double>(2, 2) = var_I_rr.at<double>(y, x);

			sigma = sigma + eps*cv::Mat::ones(3, 3, CV_64FC1);

			cov_Ip.at<double>(0, 0) = cov_Ip_b.at<double>(y, x);
			cov_Ip.at<double>(0, 1) = cov_Ip_g.at<double>(y, x);
			cov_Ip.at<double>(0, 2) = cov_Ip_r.at<double>(y, x);

			cov_Ip = cov_Ip*(sigma.inv());

			a[0].at<double>(y, x) = cov_Ip.at<double>(0, 0);
			a[1].at<double>(y, x) = cov_Ip.at<double>(0, 1);
			a[2].at<double>(y, x) = cov_Ip.at<double>(0, 2);



		}
	}

	//b = mean_p - a(:, :, 1) .* mean_I_r - a(:, :, 2) .* mean_I_g - a(:, :, 3) .* mean_I_b; % Eqn. (15) in the paper;

	cv::Mat b = mean_p - a.at(0).mul(mean_I_b) - a.at(1).mul(mean_I_g) - a.at(2).mul(mean_I_r);


	//q = (boxfilter(a(:, :, 1), r).* I(:, :, 1)...
	//+ boxfilter(a(:, :, 2), r).* I(:, :, 2)...
	//+ boxfilter(a(:, :, 3), r).* I(:, :, 3)...
	//+ boxfilter(b, r)) ./ N;  % Eqn. (16) in the paper;
	cv::Mat q, q1, q2, q3, q4;
	q1 = boxfilter(a.at(0), r);
	q2 = boxfilter(a.at(1), r);
	q3 = boxfilter(a.at(2), r);
	q4 = boxfilter(b, r);
	q = q1.mul(I_bgr[0]) + q2.mul(I_bgr[1]) + q3.mul(I_bgr[2]) + q4 / N;

	return q;


}
