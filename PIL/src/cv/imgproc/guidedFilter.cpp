
#include <opencv2/imgproc/imgproc.hpp>

#include "guidedFilter.h"

/**
 * GUIDEDFILTER   O(1) time implementation of guided filter.
 *
 *      - guidance image: I (should be a gray-scale/single channel image)
 *      - filtering input image: p (should be a gray-scale/single channel image)
 *      - local window radius: r
 *      - regularization parameter: eps
 **/
cv::Mat guidedFilter(cv::Mat &I, cv::Mat &p, int r, double eps)
{
    // convert image to double array
    cv::Mat _I;
    I.convertTo(_I, CV_64FC1);
    I = _I;

    cv::Mat _p;
    p.convertTo(_p, CV_64FC1);
    p = _p;

    //[hei, wid] = size(I);
    int hei = _I.rows;
    int wid = _I.cols;

    //N = boxfilter(ones(hei, wid), r); % the size of each local patch; N=(2r+1)^2 except for boundary pixels.
    cv::Mat N;
    cv::boxFilter(cv::Mat::ones(hei, wid, _I.type()), N, CV_64FC1, cv::Size(r, r));

    //mean_I = boxfilter(I, r) ./ N;
    cv::Mat mean_I;
    cv::boxFilter(_I, mean_I, CV_64FC1, cv::Size(r, r));

    //mean_p = boxfilter(p, r) ./ N;
    cv::Mat mean_p;
    cv::boxFilter(_p, mean_p, CV_64FC1, cv::Size(r, r));

    //mean_Ip = boxfilter(I.*p, r) ./ N;
    cv::Mat mean_Ip;
    cv::boxFilter(_I.mul(_p), mean_Ip, CV_64FC1, cv::Size(r, r));

    //cov_Ip = mean_Ip - mean_I .* mean_p; % this is the covariance of (I, p) in each local patch.
    cv::Mat cov_Ip = mean_Ip - mean_I.mul(mean_p);

    //mean_II = boxfilter(I.*I, r) ./ N;
    cv::Mat mean_II;
    cv::boxFilter(_I.mul(_I), mean_II, CV_64FC1, cv::Size(r, r));

    //var_I = mean_II - mean_I .* mean_I;
    cv::Mat var_I = mean_II - mean_I.mul(mean_I);

    //a = cov_Ip ./ (var_I + eps); % Eqn. (5) in the paper;
    cv::Mat a = cov_Ip/(var_I + eps);

    //b = mean_p - a .* mean_I; % Eqn. (6) in the paper;
    cv::Mat b = mean_p - a.mul(mean_I);

    //mean_a = boxfilter(a, r) ./ N;
    cv::Mat mean_a;
    cv::boxFilter(a, mean_a, CV_64FC1, cv::Size(r, r));
    mean_a = mean_a/N;

    //mean_b = boxfilter(b, r) ./ N;
    cv::Mat mean_b;
    cv::boxFilter(b, mean_b, CV_64FC1, cv::Size(r, r));
    mean_b = mean_b/N;

    //q = mean_a .* I + mean_b; % Eqn. (8) in the paper;
    cv::Mat q = mean_a.mul(_I) + mean_b;

    return q;
}
