$(function () {
    Highcharts.chart('container1', {
        title: {
            text: 'Teplota',
            x: -20 //center
        },

        xAxis: {
            categories: ['Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun',
                'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec']
        },
        yAxis: {
            title: {
                text: 'Teplota (C°)'
            },
            plotLines: [{
                value: 0,
                width: 1,
                color: '#808080'
            }]
        },
        tooltip: {
            valueSuffix: '°C'
        },

        series: [{
            name: 'Bratislava',
            data: [7.0, 6.9, 9.5, 14.5, 18.2, 21.5, 25.2, 26.5, 23.3, 18.3, 13.9, 19.6]
        }]
    });
});

$(function () {
    Highcharts.chart('container2', {
        title: {
            text: 'Tlak',
            x: -20 //center
        },

        xAxis: {
            categories: ['Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun',
                'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec']
        },
        yAxis: {
            title: {
                text: 'Tlak'
            },
            plotLines: [{
                value: 0,
                width: 1,
                color: '#808080'
            }]
        },
        tooltip: {
            valueSuffix: '°C'
        },
        series: [{
            name: 'Tokyo',
            data: [7.0, 6.9, 9.5, 14.5, 18.2, 21.5, 25.2, 26.5, 23.3, 18.3, 13.9, 9.6]
        }
    ]
    });
});

$(function () {
    Highcharts.chart('container3', {
        title: {
            text: 'Vlhkost',
            x: -20 //center
        },

        xAxis: {
            categories: ['Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun',
                'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec']
        },
        yAxis: {
            title: {
                text: 'Vlhkost (%)'
            },
            plotLines: [{
                value: 0,
                width: 1,
                color: '#808080'
            }]
        },
        tooltip: {
            valueSuffix: '°C'
        },

        series: [{
            name: 'Tokyo',
            data: [7.0, 6.9, 9.5, 14.5, 18.2, 21.5, 25.2, 26.5, 23.3, 18.3, 13.9, 9.6]
        }]
    });
});

/*galeria*/

    var galleries = $('.obsah');
        galleries.hide();

        var selected = $('.menu').find('.selected'),
            selectedGallery;

        function showGallery(selected){
            if ( selected.length ) {
            var id = selected.find('a, pressure').attr('href');
            selectedGallery = $(id);
        }
        var newGallery = selectedGallery.length ? selectedGallery : galleries.eq(0);
        galleries.not(newGallery).hide();
        newGallery.fadeIn();

        }

        showGallery(selected);

        $('.navbar a').on('click',function(event){
            var li = $(this).parent();

                li.addClass('selected')
                .siblings().removeClass('selected');
                showGallery(li);
                event.preventDefault();
});